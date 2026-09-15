#include "kinect.h"
#include "body.h"
#include "vector.h"

#include "../internal.h"
#include "../events.h"

#include <wrl/client.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <vector>
#include <cstddef>
#include <new>
#include <ranges>
#include <optional>

using Microsoft::WRL::ComPtr;
using Clock = std::chrono::steady_clock;

#pragma region "Stream"

/// <summary>
/// Represents a published frame.
/// </summary>
struct Frame final {

    /// <summary>
    /// The buffer that stores the frame contents.
    /// </summary>
    std::vector<std::byte> buffer{};

    /// <summary>
    /// Stores the time of arrival as a relative timestamp in milliseconds.
    /// </summary>
    int64_t timestamp{};

    /// <summary>
    /// Stores the generation of the frame.
    /// </summary>
    uint64_t generation{};

    /// <summary>
    /// Stores the floor clip plane for a body stream.
    /// </summary>
    UnectVector4 floorClipPlane{};

    /// <summary>
    /// Stores the number of tracked bodies in a body stream.
    /// </summary>
    int32_t bodyCount{};

};

/// <summary>
/// Stores meta-data about a stream.
/// </summary>
struct StreamInfo final {

    /// <summary>
    /// Stores the stream width in pixels.
    /// </summary>
    int32_t width{};

    /// <summary>
    /// Stores the stream height in pixels.
    /// </summary>
    int32_t height{};

    /// <summary>
    /// Stores the pixel depth of the stream.
    /// </summary>
    uint32_t bytesPerPixel{};

    /// <summary>
    /// The target format for the stream image.
    /// </summary>
    //UnectColorFormat format{};

    /// <summary>
    /// Returns the number of pixels in the stream.
    /// </summary>
    /// <returns>The number of pixels in the stream.</returns>
    inline int32_t pixels() const noexcept {
        return width * height;
    }

    /// <summary>
    /// Returns the size of the stream in bytes.
    /// </summary>
    /// <returns>The size of the stream image in bytes.</returns>
    inline uint32_t size() const noexcept {
        return static_cast<uint32_t>(pixels()) * bytesPerPixel;
    }

    /// <summary>
    /// Returns a stream info from a frame description.
    /// </summary>
    /// <param name="frameDesc">The frame description.</param>
    /// <returns>The stream info built from the description, or `std::nullopt`, if no conversion was possible.</returns>
    static std::optional<StreamInfo> describeFrom(IFrameDescription* frameDesc/*, UnectColorFormat format*/) {
        if (!frameDesc) 
            return {};

        // Get the frame image dimensions.
        StreamInfo info{};

        if (FAILED(frameDesc->get_Width(&info.width)) || FAILED(frameDesc->get_Height(&info.height)) || FAILED(frameDesc->get_BytesPerPixel(&info.bytesPerPixel)))
            return {};
        else
            return info;
    }

};

/// <summary>
/// Represents a stream from a sensor source.
/// </summary>
struct Stream final {

    /// <summary>
    /// Locks access to the streams <see cref="syncContext" />.
    /// </summary>
    std::mutex lock{};

    /// <summary>
    /// Stores the synchronization context for a stream, containing information about the current and locked frames.
    /// </summary>
    struct SyncContext final {

        /// <summary>
        /// The index of the published frame.
        /// </summary>
        int64_t publishedFrame { -1 };

        /// <summary>
        /// The index of the locked frame.
        /// </summary>
        int64_t lockedFrame { -1 };

        /// <summary>
        /// If set to `true`, the stream can replace the last frame, otherwise the next frame(s) are dropped until the client fetches the current frame.
        /// </summary>
        bool canReplace{ true };

    };

    /// <summary>
    /// Stores the synchronization context for the stream.
    /// </summary>
    SyncContext syncContext{};

    /// <summary>
    /// Stores the active state of the stream.
    /// </summary>
    std::atomic<bool> isEnabled{};

    /// <summary>
    /// Stores the information about the stream.
    /// </summary>
    StreamInfo info{};

    /// <summary>
    /// Stores the currently published frames.
    /// </summary>
    std::vector<Frame> frames{};

    /// <summary>
    /// Stores the generation of the stream.
    /// </summary>
    std::atomic<uint64_t> generation{};

    /// <summary>
    /// Stores the total number of frames received through the stream.
    /// </summary>
    std::atomic<uint64_t> receivedFrames{};

    /// <summary>
    /// Stores the total number of frames dropped by the stream.
    /// </summary>
    std::atomic<uint64_t> droppedFrames{};

    /// <summary>
    /// Stores the current frame-rate in frames per second.
    /// </summary>
    std::atomic<float> framerate{};

    /// <summary>
    /// Stores the latency of the last frame in milliseconds.
    /// </summary>
    std::atomic<float> lastFrameLatency{};

    /// <summary>
    /// Stores the timestamp of the last frame arrival
    /// </summary>
    Clock::time_point lastArrival{};

    /// <summary>
    /// Stores the handle to the event that signals to the stream that a new frame has arrived.
    /// </summary>Lo
    WAITABLE_HANDLE onFrameArrived{};

    /// <summary>
    /// Locks a frame for writing.
    /// </summary>
    /// <param name="stream">The stream to lock the frame on.</param>
    /// <returns>The index of the locked frame, or `-1`, if no lock could be acquired.</returns>
    inline int64_t SwapBackBuffer() {
        std::lock_guard<std::mutex> g{ lock };

        for (int64_t i{ 0 }; i < static_cast<int64_t>(frames.size()); ++i)
            if (i != syncContext.publishedFrame && i != syncContext.lockedFrame)
                return i;

        return -1;
    }

    /// <summary>
    /// Updates the current frame index as well as the latency and frame-rate statistics.
    /// </summary>
    /// <param name="currentFrameIndex">The index of the currently processed frame.</param>
    /// <param name="timestamp">The timestamp at which the frame was processed.</param>
    void Update(int64_t currentFrameIndex, Clock::time_point timestamp) {
        // Publish the current frame.
        {
            std::lock_guard<std::mutex> g{ lock };

            if (syncContext.publishedFrame >= 0 && !syncContext.canReplace)
                droppedFrames.fetch_add(1, std::memory_order_relaxed);

            syncContext.publishedFrame = currentFrameIndex;
            syncContext.canReplace = false;
        }

        // Update the latency and frame-rate statistics.
        const auto now = Clock::now();
        receivedFrames.fetch_add(1, std::memory_order_relaxed);
        lastFrameLatency.store(std::chrono::duration<float, std::milli>(now - timestamp).count(), std::memory_order_relaxed);

        if (lastArrival.time_since_epoch().count() != 0) {
            auto d = std::chrono::duration<float, std::milli>(now - lastArrival).count();

            if (d > 0.0f) {
                auto inst = 1000.0f / d;
                auto prev = framerate.load(std::memory_order_relaxed);
                framerate.store(prev == 0.0f ? inst : prev * 0.9f + inst * 0.1f, std::memory_order_relaxed);
            }
        }

        lastArrival = now;
    }

};

#pragma endregion

#pragma region "Adapter"

/// <summary>
/// Stores the pointers to the frame readers used by a <see cref="KinectAdapter" />.
/// </summary>
struct StreamReaders final {

    /// <summary>
    /// A pointer to the depth reader.
    /// </summary>
    ComPtr<IDepthFrameReader> depth{};

    /// <summary>
    /// A pointer to the color reader.
    /// </summary>
    ComPtr<IColorFrameReader> color{};

    /// <summary>
    /// A pointer to the IR stream reader.
    /// </summary>
    ComPtr<IInfraredFrameReader> infrared{};

    /// <summary>
    /// A pointer to the long exposure IR stream reader.
    /// </summary>
    ComPtr<ILongExposureInfraredFrameReader> longExposureInfrared{};

    /// <summary>
    /// A pointer to the body index reader.
    /// </summary>
    ComPtr<IBodyIndexFrameReader> bodyIndex{};

    /// <summary>
    /// A pointer to the body reader.
    /// </summary>
    ComPtr<IBodyFrameReader> body{};

};

/// <summary>
/// An adapter for the device that acts as an interface to the streaming logic.
/// </summary>
struct KinectAdapter final {

    KinectAdapter(const KinectAdapter&) = delete;
    KinectAdapter(KinectAdapter&&) noexcept = delete;
    KinectAdapter& operator=(const KinectAdapter&) = delete;
    KinectAdapter& operator=(KinectAdapter&&) noexcept = delete;

    /// <summary>
    /// A lock for the adapter members.
    /// </summary>
    std::mutex lock{};

    /// <summary>
    /// Stores the state of the adapter.
    /// </summary>
    std::atomic<bool> isRunning{ false };

    /// <summary>
    /// A handle for the sensor's availability event.
    /// </summary>
    WAITABLE_HANDLE onAvailable{};
    
    /// <summary>
    /// A handle for the mapping coordinate mapper's mapping event.
    /// </summary>
    WAITABLE_HANDLE onMapping{};

    /// <summary>
    /// An event that is raised to reset the streamers.
    /// </summary>
    HANDLE resetEvent{};

    /// <summary>
    /// The streamer worker thread.
    /// </summary>
    std::jthread streamer{};

    /// <summary>
    /// A pointer to the underlying kinect sensor.
    /// </summary>
    ComPtr<IKinectSensor> sensor{};

    /// <summary>
    /// A pointer to the underlying coordinate mapper.
    /// </summary>
    ComPtr<ICoordinateMapper> coordinateMapper{};

    /// <summary>
    /// Stores the readers that read from the sensor.
    /// </summary>
    StreamReaders readers{};

    /// <summary>
    /// Stores the individual stream instances.
    /// </summary>
    std::array<Stream, UNECT_SI_COUNT> streams{};

    /// <summary>
    /// The color format used by the color stream.
    /// </summary>
    UnectColorFormat colorFormat{ UNECT_COLOR_BGRA32 };

    /// <summary>
    /// The number of back buffers maintained by the adapter.
    /// </summary>
    int32_t bufferCount{ 3 };

private:
    /// <summary>
    /// Stores the previous body IDs.
    /// </summary>
    std::unordered_set<uint64_t> previousBodyIds{};

public:
    /// <summary>
    /// Returns the adapter singleton.
    /// </summary>
    /// <returns>A reference to the adapter singleton.</returns>
    static KinectAdapter& get() {
        alignas(KinectAdapter) static std::byte storage[sizeof(KinectAdapter)];
        static KinectAdapter* a = new (storage) KinectAdapter();

        return *a;
    }

    /// <summary>
    /// Attempts to open the streams indicated by the <paramref name="streams" /> mask.
    /// </summary>
    /// <param name="streamTypes">A mask indicating the streams to open.</param>
    /// <returns>The return code of the function.</returns>
    inline UnectResult OpenStreams(UnectStreamType streamTypes);

    /// <summary>
    /// Shuts down streaming from the adapter.
    /// </summary>
    inline void Shutdown();

    /// <summary>
    /// Allocates the frame buffer for a stream.
    /// </summary>
    /// <param name="stream">A reference to the stream to allocate.</param>
    /// <param name="frameSize">The size of a frame in the stream (in bytes).</param>
    void AllocateFrames(Stream& stream, int32_t frameSize);

    /// <summary>
    /// Streams frames for the stream identified by <paramref name="from" />.
    /// </summary>
    /// <param name="from">The index of the stream from which to fetch the frames.</param>
    void StreamFrames(UnectStreamIndex from);

    /// <summary>
    /// Handles the sensor availability event.
    /// </summary>
    void AvailabilityChangedHandler() {
        ComPtr<IIsAvailableChangedEventArgs> eventArgs{};

        if (FAILED(sensor->GetIsAvailableChangedEventData(onAvailable, &eventArgs)) || !eventArgs)
            return;

        BOOLEAN available{ FALSE };
        eventArgs->get_IsAvailable(&available);

        auto sensorState = available ? UNECT_SENSOR_AVAILABLE : UNECT_SENSOR_UNAVAILABLE;
        Internal::g_sensorState.store(sensorState, std::memory_order_relaxed);
        PushEvent(UNECT_EVENT_SENSOR_STATE, sensorState, 0);
    }

private:
    /// <summary>
    /// Reads a frame from a stream.
    /// </summary>
    /// <typeparam name="TArgs">The type of the frame arguments.</typeparam>
    /// <typeparam name="TRef">The type of the frame reference.</typeparam>
    /// <typeparam name="TFrame">The type of the frame interface.</typeparam>
    /// <typeparam name="TReader">The type of the frame reader.</typeparam>
    /// <typeparam name="FCopy">The type of the copy function.</typeparam>
    /// <param name="stream">The stream to read from.</param>
    /// <param name="reader">The reader used to read from the stream.</param>
    /// <param name="copyInto">The function that copies the frame data into the frame buffer.</param>
    template <class TArgs, class TRef, class TFrame, class TReader, class FCopy>
    inline void ReadNextFrame(Stream& stream, TReader* reader, FCopy&& copyInto) {
        Clock::time_point timestamp{};
        int64_t frameIndex{};
        
        {
            ComPtr<TArgs> args;
            ComPtr<TRef> ref;
            ComPtr<TFrame> frame;

            if (FAILED(reader->GetFrameArrivedEventData(stream.onFrameArrived, &args)) || !args)
                return;
            else if (FAILED(args->get_FrameReference(&ref)) || !ref)
                return;
            else if (FAILED(ref->AcquireFrame(&frame)) || !frame)
                return;

            // Store the current timestamp and swap the back buffer of the stream.
            timestamp = Clock::now();
            frameIndex = stream.SwapBackBuffer();

            // Check if we could actually acquire a frame.
            if (frameIndex < 0)
                return;

            auto& frameRef = stream.frames.at(frameIndex);

            if (!copyInto(frame.Get(), frameRef))
                return;

            // Compute the timestamp and generation.
            TIMESPAN rt = 0;
            frame->get_RelativeTime(&rt);
            frameRef.timestamp = rt / 10'000; // Convert to ms.
            frameRef.generation = stream.generation.fetch_add(1, std::memory_order_relaxed) + 1;
        }

        // Update the stream.
        stream.Update(frameIndex, timestamp);
    }

    /// <summary>
    /// Reads a body from a stream.
    /// </summary>
    /// <param name="stream">The stream to read from.</param>
    inline void ReadNextBody(Stream& stream) {
        Clock::time_point timestamp{};
        int64_t frameIndex{};

        {
            ComPtr<IBodyFrameArrivedEventArgs> args;
            ComPtr<IBodyFrameReference> ref;
            ComPtr<IBodyFrame> frame;

            if (FAILED(readers.body->GetFrameArrivedEventData(stream.onFrameArrived, &args)) || !args)
                return;
            else if (FAILED(args->get_FrameReference(&ref)) || !ref)
                return;
            else if (FAILED(ref->AcquireFrame(&frame)) || !frame)
                return;

            // Store the current timestamp, get the body data and swap the back buffer of the stream.
            auto timestamp = Clock::now();

            struct BodyData final {
                IBody* raw[BODY_COUNT] = {};

                ~BodyData() {
                    release();
                }

                inline void release() {
                    for (auto body : raw) {
                        if (body)
                            body->Release();

                        body = {};
                    }
                }
            } bodyData{};

            if (FAILED(frame->GetAndRefreshBodyData(BODY_COUNT, bodyData.raw)))
                return;

            auto frameIndex = stream.SwapBackBuffer();

            if (frameIndex < 0)
                return;

            // Copy the body into the frame.
            auto& frameRef = stream.frames.at(frameIndex);
            Body* bodies = std::start_lifetime_as<Body>(frameRef.buffer.data());

            // Convert the bodies.
            std::unordered_set<uint64_t> visibleBodyIds{};
            int trackedBodies{};

            for (int i{}; i < BODY_COUNT; ++i) {
                if (!bodyData.raw[i])
                    bodies[i] = {};
                else {
                    bodies[i] = ::ConvertFrom(bodyData.raw[i]);

                    if (bodies[i].isTracked) {
                        ++trackedBodies;
                        visibleBodyIds.insert(bodies[i].trackingId);
                    }
                }
            }

            // Convert the floor clip plane.
            Vector4 floor{};
            frame->get_FloorClipPlane(&floor);
            frameRef.floorClipPlane = { floor.x, floor.y, floor.z, floor.w };
            frameRef.bodyCount = trackedBodies;

            // Compute the timestamp and generation.
            TIMESPAN rt = 0;
            frame->get_RelativeTime(&rt);
            frameRef.timestamp = rt / 10'000; // Convert to ms.
            frameRef.generation = stream.generation.fetch_add(1, std::memory_order_relaxed) + 1;

            // Update the previously visible body IDs and publish events, if body visibility changed.
            for (auto id : visibleBodyIds)
                if (previousBodyIds.find(id) == previousBodyIds.end())
                    PushEvent(UNECT_EVENT_BODY_ENTERED, 0, id);

            for (auto id : previousBodyIds)
                if (visibleBodyIds.find(id) == visibleBodyIds.end())
                    PushEvent(UNECT_EVENT_BODY_LEFT, 0, id);

            previousBodyIds = std::move(visibleBodyIds);
        }

        // Update the stream.
        stream.Update(frameIndex, timestamp);
    }

private:
    /// <summary>
    /// Creates a new adapter instance.
    /// </summary>
    KinectAdapter() = default;

    /// <summary>
    /// The purposefully deleted destructor of the adapter.
    /// </summary>
    ~KinectAdapter() = delete;

};

UnectResult KinectAdapter::OpenStreams(UnectStreamType streamTypes) {
    std::vector<UnectStreamIndex> openStreams{};

    // Fail handler.
    auto fail = [&](UnectResult reason) {
        // Reset streams.
        for (auto& stream : openStreams | std::views::transform([this](auto type) -> Stream& { return streams[type]; })) {
            stream.isEnabled.store(false, std::memory_order_release);
            stream.onFrameArrived = {};
            stream.frames.clear();
        }

        // Reset readers.
        readers = {};

        return reason;
    };

    // Checks if a stream should be opened
    auto shouldOpen = [&](UnectStreamType type, UnectStreamIndex index) { return (streamTypes & type) && !streams[index].isEnabled.load(std::memory_order_acquire); };

    // Color format conversion
    auto convertColorFormat = [](UnectColorFormat format) {
        // NOTE: The default branch includes unsupported formats.
        switch (format) {
        default:
        case UNECT_COLOR_BGRA32:
            return ColorImageFormat_Bgra;
        case UNECT_COLOR_RGBA32:
            return ColorImageFormat_Rgba;
        case UNECT_COLOR_YUY2:
            return ColorImageFormat_Yuy2;
        }
    };

    // Depth image stream.
    if (shouldOpen(UNECT_STREAM_DEPTH, UNECT_SI_DEPTH)) {
        auto& stream = streams.at(UNECT_SI_DEPTH);

        ComPtr<IDepthFrameSource> source;
        ComPtr<IFrameDescription> frameDesc;

        if (FAILED(sensor->get_DepthFrameSource(&source)) || !source)
            return fail(UNECT_E_FAIL);

        if (FAILED(source->get_FrameDescription(&frameDesc)))
            return fail(UNECT_E_FAIL);
        else if (auto info = StreamInfo::describeFrom(frameDesc.Get()); info.has_value())
            stream.info = info.value();
        else
            fail(UNECT_E_FAIL);

        if (FAILED(source->OpenReader(&readers.depth))) 
            return fail(UNECT_E_FAIL);

        if (FAILED(readers.depth->SubscribeFrameArrived(&stream.onFrameArrived))) 
            return fail(UNECT_E_FAIL);

        AllocateFrames(stream, stream.info.bytesPerPixel);
        stream.isEnabled.store(true, std::memory_order_release); 
        openStreams.push_back(UNECT_SI_DEPTH);
    }

    // Color image stream.
    if (shouldOpen(UNECT_STREAM_COLOR, UNECT_SI_COLOR)) {
        auto& stream = streams.at(UNECT_SI_COLOR);

        ComPtr<IColorFrameSource> source;
        ComPtr<IFrameDescription> frameDesc;

        if (FAILED(sensor->get_ColorFrameSource(&source)) || !source) 
            return fail(UNECT_E_FAIL);

        if (FAILED(source->CreateFrameDescription(convertColorFormat(colorFormat), &frameDesc)))
            return fail(UNECT_E_FAIL);
        else if (auto info = StreamInfo::describeFrom(frameDesc.Get()); info.has_value())
            stream.info = info.value();
        else
            fail(UNECT_E_FAIL);

        if (FAILED(source->OpenReader(&readers.color)))
            return fail(UNECT_E_FAIL);

        if (FAILED(readers.color->SubscribeFrameArrived(&stream.onFrameArrived)))
            return fail(UNECT_E_FAIL);

        AllocateFrames(stream, stream.info.bytesPerPixel);
        stream.isEnabled.store(true, std::memory_order_release); 
        openStreams.push_back(UNECT_SI_COLOR);
    }

    // Infrared image stream.
    if (shouldOpen(UNECT_STREAM_INFRARED, UNECT_SI_INFRARED)) {
        auto& stream = streams.at(UNECT_SI_INFRARED);

        ComPtr<IInfraredFrameSource> source; 
        ComPtr<IFrameDescription> frameDesc;

        if (FAILED(sensor->get_InfraredFrameSource(&source)) || !source) 
            return fail(UNECT_E_FAIL);

        if (FAILED(source->get_FrameDescription(&frameDesc)))
            return fail(UNECT_E_FAIL);
        else if (auto info = StreamInfo::describeFrom(frameDesc.Get()); info.has_value())
            stream.info = info.value();
        else
            fail(UNECT_E_FAIL);

        if (FAILED(source->OpenReader(&readers.infrared))) 
            return fail(UNECT_E_FAIL);

        if (FAILED(readers.infrared->SubscribeFrameArrived(&stream.onFrameArrived))) 
            return fail(UNECT_E_FAIL);

        AllocateFrames(stream, stream.info.bytesPerPixel);
        stream.isEnabled.store(true, std::memory_order_release);
        openStreams.push_back(UNECT_SI_INFRARED);
    }

    // Long exposure IR stream.
    if (shouldOpen(UNECT_STREAM_LONG_EXPOSURE_IR, UNECT_SI_LONG_EXPOSURE_IR)) {
        auto& stream = streams.at(UNECT_SI_LONG_EXPOSURE_IR);

        ComPtr<ILongExposureInfraredFrameSource> source; 
        ComPtr<IFrameDescription> frameDesc;

        if (FAILED(sensor->get_LongExposureInfraredFrameSource(&source)) || !source)
            return fail(UNECT_E_FAIL);

        if (FAILED(source->get_FrameDescription(&frameDesc)))
            return fail(UNECT_E_FAIL);
        else if (auto info = StreamInfo::describeFrom(frameDesc.Get()); info.has_value())
            stream.info = info.value();
        else
            fail(UNECT_E_FAIL);

        if (FAILED(source->OpenReader(&readers.longExposureInfrared)))
            return fail(UNECT_E_FAIL);

        if (FAILED(readers.longExposureInfrared->SubscribeFrameArrived(&stream.onFrameArrived)))
            return fail(UNECT_E_FAIL);

        AllocateFrames(stream, stream.info.bytesPerPixel);
        stream.isEnabled.store(true, std::memory_order_release);
        openStreams.push_back(UNECT_SI_LONG_EXPOSURE_IR);
    }

    // Body index image stream.
    if (shouldOpen(UNECT_STREAM_BODY_INDEX, UNECT_SI_BODY_INDEX)) {
        auto& stream = streams.at(UNECT_SI_BODY_INDEX);

        ComPtr<IBodyIndexFrameSource> source; 
        ComPtr<IFrameDescription> frameDesc;

        if (FAILED(sensor->get_BodyIndexFrameSource(&source)) || !source)
            return fail(UNECT_E_FAIL);

        if (FAILED(source->get_FrameDescription(&frameDesc)))
            return fail(UNECT_E_FAIL);
        else if (auto info = StreamInfo::describeFrom(frameDesc.Get()); info.has_value())
            stream.info = info.value();
        else
            fail(UNECT_E_FAIL);

        if (FAILED(source->OpenReader(&readers.bodyIndex)))
            return fail(UNECT_E_FAIL);

        if (FAILED(readers.bodyIndex->SubscribeFrameArrived(&stream.onFrameArrived)))
            return fail(UNECT_E_FAIL);

        AllocateFrames(stream, stream.info.bytesPerPixel);
        stream.isEnabled.store(true, std::memory_order_release);
        openStreams.push_back(UNECT_SI_BODY_INDEX);
    }

    // Body stream.
    if (shouldOpen(UNECT_STREAM_BODY, UNECT_SI_BODY)) {
        auto& stream = streams.at(UNECT_SI_BODY);

        ComPtr<IBodyFrameSource> source;

        if (FAILED(sensor->get_BodyFrameSource(&source)) || !source)
            return fail(UNECT_E_FAIL);

        if (FAILED(source->OpenReader(&readers.body)))
            return fail(UNECT_E_FAIL);

        if (FAILED(readers.body->SubscribeFrameArrived(&stream.onFrameArrived)))
            return fail(UNECT_E_FAIL);

        stream.info = { 
            UNECT_BODY_COUNT, 1,
            static_cast<uint32_t>(sizeof(Body)) 
        };

        AllocateFrames(stream, stream.info.bytesPerPixel);
        stream.isEnabled.store(true, std::memory_order_release);
        openStreams.push_back(UNECT_SI_BODY);
    }

    return UNECT_OK;
}

void KinectAdapter::Shutdown() {
    // Unsubscribe all streams.
    if (readers.depth && streams[UNECT_SI_DEPTH].onFrameArrived)
        readers.depth->UnsubscribeFrameArrived(streams[UNECT_SI_DEPTH].onFrameArrived);

    if (readers.color && streams[UNECT_SI_COLOR].onFrameArrived)
        readers.color->UnsubscribeFrameArrived(streams[UNECT_SI_COLOR].onFrameArrived);

    if (readers.infrared && streams[UNECT_SI_INFRARED].onFrameArrived)
        readers.infrared->UnsubscribeFrameArrived(streams[UNECT_SI_INFRARED].onFrameArrived);

    if (readers.longExposureInfrared && streams[UNECT_SI_LONG_EXPOSURE_IR].onFrameArrived)
        readers.longExposureInfrared->UnsubscribeFrameArrived(streams[UNECT_SI_LONG_EXPOSURE_IR].onFrameArrived);

    if (readers.bodyIndex && streams[UNECT_SI_BODY_INDEX].onFrameArrived)
        readers.bodyIndex->UnsubscribeFrameArrived(streams[UNECT_SI_BODY_INDEX].onFrameArrived);

    if (readers.body && streams[UNECT_SI_BODY].onFrameArrived)
        readers.body->UnsubscribeFrameArrived(streams[UNECT_SI_BODY].onFrameArrived);

    // Unsubscribe coordinate mapper from mapping event.
    if (coordinateMapper && onMapping)
        coordinateMapper->UnsubscribeCoordinateMappingChanged(onMapping);
    
    onMapping = {};

    // Unsubscribe sensor from availability event.
    if (sensor && onAvailable)
        sensor->UnsubscribeIsAvailableChanged(onAvailable);

    onAvailable = {};

    // Reset readers and release coordinate mapper and sensor pointers.
    readers = {};
    coordinateMapper.Reset();
    sensor.Reset();

    // Release all streams.
    for (auto& stream : streams) {
        std::lock_guard<std::mutex> lock{ stream.lock };

        stream.isEnabled.store(false, std::memory_order_relaxed);
        stream.onFrameArrived = 0;
        stream.frames.clear();
        stream.syncContext = {};
        stream.info = StreamInfo{};
        stream.lastArrival = {};
        stream.framerate.store(0.0f, std::memory_order_relaxed);
        stream.lastFrameLatency.store(0.0f, std::memory_order_relaxed);
    }

    // Delete the previous body IDs.
    previousBodyIds.clear();

    // Release the reset event.
    if (resetEvent) { 
        ::CloseHandle(resetEvent); 
        resetEvent = {};
    }
}

void KinectAdapter::AllocateFrames(Stream& stream, int32_t byteCount) {
    stream.frames = std::views::iota(0, bufferCount)
        | std::views::transform([byteCount](auto i) { return Frame { .buffer = std::vector<std::byte>(byteCount) }; })
        | std::ranges::to<std::vector>();

    stream.syncContext = {};
}

void KinectAdapter::StreamFrames(UnectStreamIndex from) {
    auto& stream = streams[from];

    switch (from) 
    {
    case UNECT_SI_DEPTH:
        ReadNextFrame<IDepthFrameArrivedEventArgs, IDepthFrameReference, IDepthFrame>(stream, readers.depth.Get(), [&](IDepthFrame* ptr, Frame& f) {
                return SUCCEEDED(ptr->CopyFrameDataToArray(static_cast<UINT>(stream.info.pixels()), reinterpret_cast<UINT16*>(f.buffer.data())));
            });
        break;
    case UNECT_SI_COLOR:
        ReadNextFrame<IColorFrameArrivedEventArgs, IColorFrameReference, IColorFrame>(stream, readers.color.Get(), [&](IColorFrame* ptr, Frame& f) {
                auto cap = static_cast<UINT>(f.buffer.size());

                return colorFormat == UNECT_COLOR_YUY2 ?
                    SUCCEEDED(ptr->CopyRawFrameDataToArray(cap, reinterpret_cast<BYTE*>(f.buffer.data()))) :
                    SUCCEEDED(ptr->CopyConvertedFrameDataToArray(cap, reinterpret_cast<BYTE*>(f.buffer.data()), colorFormat == UNECT_COLOR_RGBA32 ? ColorImageFormat_Rgba : ColorImageFormat_Bgra));
            });
        break;
    case UNECT_SI_INFRARED:
        ReadNextFrame<IInfraredFrameArrivedEventArgs, IInfraredFrameReference, IInfraredFrame>(stream, readers.infrared.Get(), [&](IInfraredFrame* ptr, Frame& f) {
                return SUCCEEDED(ptr->CopyFrameDataToArray(static_cast<UINT>(stream.info.pixels()), reinterpret_cast<UINT16*>(f.buffer.data())));
            });
        break;
    case UNECT_SI_LONG_EXPOSURE_IR:
        ReadNextFrame<ILongExposureInfraredFrameArrivedEventArgs, ILongExposureInfraredFrameReference, ILongExposureInfraredFrame>(stream, readers.longExposureInfrared.Get(), [&](ILongExposureInfraredFrame* ptr, Frame& f) {
                return SUCCEEDED(ptr->CopyFrameDataToArray(static_cast<UINT>(stream.info.pixels()), reinterpret_cast<UINT16*>(f.buffer.data())));
            });
        break;
    case UNECT_SI_BODY_INDEX:
        ReadNextFrame<IBodyIndexFrameArrivedEventArgs, IBodyIndexFrameReference, IBodyIndexFrame>(stream, readers.bodyIndex.Get(), [&](IBodyIndexFrame* ptr, Frame& f) {
                return SUCCEEDED(ptr->CopyFrameDataToArray(static_cast<UINT>(stream.info.pixels()), reinterpret_cast<BYTE*>(f.buffer.data())));
            });
        break;
    case UNECT_SI_BODY:
        ReadNextBody(stream);
        break;
    default:
        break;
    }
}

#pragma endregion

#pragma region "Streamer thread"

/// <summary>
/// The main worker thread of the streamer.
/// </summary>
/// <param name="stopToken">A token to signal that the streamer should stop.</param>
void StreamerThread(std::stop_token stopToken) {
    const auto coInitResult = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    enum class Command : int32_t {
        Reset = -1,
        AvailabilityChanged = -2,
        MappingChanged = -3
    };

    auto& adapter = KinectAdapter::get();
    std::vector<HANDLE> handles{};
    std::vector<Command> commands{};
    std::stop_callback onStop(stopToken, [&] { ::SetEvent(adapter.resetEvent); });
    auto asHandle = [](WAITABLE_HANDLE h) { return reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(h)); };

    while (!stopToken.stop_requested()) {
        // Cache incoming stream commands.
        {
            std::lock_guard<std::mutex> lock(adapter.lock);

            handles.clear(); 
            commands.clear();
            handles.push_back(adapter.resetEvent); 
            commands.push_back(Command::Reset);

            if (adapter.onAvailable) { 
                handles.push_back(asHandle(adapter.onAvailable));
                commands.push_back(Command::AvailabilityChanged);
            }
            
            if (adapter.onMapping) { 
                handles.push_back(asHandle(adapter.onMapping));
                commands.push_back(Command::MappingChanged);
            }
            
            for (int32_t i{}; i < UNECT_SI_COUNT; ++i) {
                auto& stream = adapter.streams.at(i);

                if (stream.isEnabled.load(std::memory_order_acquire) && stream.onFrameArrived) {
                    handles.push_back(asHandle(stream.onFrameArrived));
                    commands.push_back(static_cast<Command>(i));
                }
            }
        }
        
        // Wait for the event handles.
        auto event = ::WaitForMultipleObjects(static_cast<DWORD>(handles.size()), handles.data(), FALSE, INFINITE);

        // If waiting for the event handles failed, we can't recover here.
        if (event < WAIT_OBJECT_0 || event >= WAIT_OBJECT_0 + handles.size())
            break;

        auto command = commands[event - WAIT_OBJECT_0];

        // Handle the commands.
        switch (command) {
        case Command::Reset:
            continue;
        case Command::AvailabilityChanged:
            adapter.AvailabilityChangedHandler();
            continue;
        case Command::MappingChanged:
            Internal::g_mappingGeneration.fetch_add(1u, std::memory_order_release);
            PushEvent(UNECT_EVENT_MAPPING_CHANGED, 0, 0);
            continue;
        default:
            adapter.StreamFrames(static_cast<UnectStreamIndex>(std::to_underlying(command)));
            continue;
        }
    }
    
    if (SUCCEEDED(coInitResult))
        ::CoUninitialize();
}

#pragma endregion

#pragma region "Public interface"

UnectResult StartStreaming(IKinectSensor* sensor, const UnectSessionDesc& sessionDesc) {
    if (!sensor) 
        return UNECT_E_INVALID_ARG;

    // Acquire the adapter instance.
    auto& adapter = KinectAdapter::get();
    std::lock_guard<std::mutex> lock{ adapter.lock };

    // Check if the adapter is already running. Calling this function twice is an error.
    if (adapter.isRunning.load(std::memory_order_relaxed))
        return UNECT_E_FAIL;

    // Store the sensor instance and the relevant session descriptor fields.
    adapter.sensor = sensor;
    adapter.colorFormat = sessionDesc.colorFormat;
    adapter.bufferCount = std::clamp<int32_t>(sessionDesc.bufferCount, UNECT_MIN_BUFFERS, UNECT_MAX_BUFFERS);

    if (SUCCEEDED(sensor->get_CoordinateMapper(&adapter.coordinateMapper)) && adapter.coordinateMapper)
        adapter.coordinateMapper->SubscribeCoordinateMappingChanged(&adapter.onMapping);

    sensor->SubscribeIsAvailableChanged(&adapter.onAvailable);

    // Try to open the streams.
    if (auto result = adapter.OpenStreams(sessionDesc.streams); result != UNECT_OK) {
        adapter.Shutdown();
        return result;
    }

    // Try to create an event that can be raised to reset the streamer thread.
    if (adapter.resetEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr); !adapter.resetEvent) {
        adapter.Shutdown();
        return UNECT_E_FAIL;
    }

    adapter.isRunning.store(true, std::memory_order_release);
    adapter.streamer = std::jthread(&::StreamerThread);

    return UNECT_OK;
}

UnectResult WidenStreams(IKinectSensor* sensor, UnectStreamType streams) {
    auto& adapter = KinectAdapter::get();
    std::lock_guard<std::mutex> lock{ adapter.lock };

    // Validate the arguments.
    if (!adapter.isRunning.load(std::memory_order_relaxed))
        return UNECT_E_FAIL;

    if (sensor != adapter.sensor.Get())
        return UNECT_E_INVALID_ARG;

    if (streams == UNECT_STREAM_NONE)
        return UNECT_OK;

    // Check if we can open the streams.
    if (auto result = adapter.OpenStreams(streams); result != UNECT_OK)
        return result;

    // Raise the reset event.
    ::SetEvent(adapter.resetEvent);
    
    return UNECT_OK;
}

void StopStreams() {
    auto& adapter = KinectAdapter::get();

    if (adapter.isRunning.exchange(false, std::memory_order_acq_rel)) {
        // Stop the streamer.
        if (adapter.streamer.request_stop())
            adapter.streamer.join();
    }

    std::lock_guard<std::mutex> lock{ adapter.lock };
    adapter.Shutdown();
}

void ForceUnlockStreams() {
    auto& adapter = KinectAdapter::get();

    for (auto& stream : adapter.streams) {
        std::lock_guard<std::mutex> lock{ stream.lock };
        stream.syncContext.lockedFrame = -1;
    }
}

UnectResult Unect_GetStreamStats(UnectSessionHandle session, UnectStreamIndex streamIndex, UnectStreamStats* stats) {
    if (!stats) 
        return UNECT_E_INVALID_ARG;

    *stats = UnectStreamStats{};

    if (streamIndex < 0 || streamIndex >= UNECT_SI_COUNT)
        return UNECT_E_INVALID_ARG;

    if (!Unect_SessionValid(session))
        return UNECT_E_STALE_SESSION;

    auto& adapter = KinectAdapter::get();
    auto& stream = adapter.streams[streamIndex];

    if (!stream.isEnabled.load(std::memory_order_acquire))
        return UNECT_E_STREAM_NOT_ENABLED;

    stats->framesArrived    = stream.receivedFrames.load(std::memory_order_relaxed);
    stats->framesDropped    = stream.droppedFrames.load(std::memory_order_relaxed);
    stats->framerate        = stream.framerate.load(std::memory_order_relaxed);
    stats->lastLatency      = stream.lastFrameLatency.load(std::memory_order_relaxed);

    return UNECT_OK;
}

#pragma endregion

#pragma region "Library interface"

inline UnectResult GetStream(UnectSessionHandle session, UnectStreamIndex streamIndex, Stream** s) {
    *s = {};

    if (!Unect_SessionValid(session))
        return UNECT_E_STALE_SESSION;

    if (streamIndex < 0 || streamIndex >= UNECT_SI_COUNT)
        return UNECT_E_INVALID_ARG;

    auto& adapter = KinectAdapter::get();
    auto& stream = adapter.streams[streamIndex];

    if (!stream.isEnabled.load(std::memory_order_acquire))
        return UNECT_E_STREAM_NOT_ENABLED;

    *s = &stream;
    return UNECT_OK;
}

UnectResult Unect_GetStreamInfo(UnectSessionHandle session, UnectStreamIndex stream, UnectStreamInfo* info) {
    if (!info)
        return UNECT_E_INVALID_ARG;

    *info = {};
    Stream* s{};

    if (auto result = ::GetStream(session, stream, &s); result != UNECT_OK)
        return result;

    *info = {
        .width = s->info.width,
        .height = s->info.height,
        .bytesPerPixel = s->info.bytesPerPixel,
        .pixelCount = s->info.pixels(),
        .totalSize = s->info.size()
    };

    return UNECT_OK;
}

uint64_t Unect_PeekGeneration(UnectSessionHandle session, UnectStreamIndex stream) {
    Stream* s{};

    if (auto result = ::GetStream(session, stream, &s); result != UNECT_OK)
        return 0u;

    return s->generation.load(std::memory_order_acquire);
}

UnectResult Unect_LockImage(UnectSessionHandle session, UnectStreamIndex stream, UnectImageView* image) {
    if (!image)
        return UNECT_E_INVALID_ARG;

    *image = {};

    if (stream == UNECT_SI_BODY)
        return UNECT_E_INVALID_ARG;

    Stream* s{};

    if (auto result = ::GetStream(session, stream, &s); result != UNECT_OK)
        return result;

    std::lock_guard<std::mutex> lock{ s->lock };

    if (s->syncContext.lockedFrame >= 0) 
        return UNECT_E_ALREADY_LOCKED;
    else if (s->syncContext.publishedFrame < 0)
        return UNECT_NO_FRAME;

    s->syncContext.lockedFrame = s->syncContext.publishedFrame;
    s->syncContext.canReplace = true;
    
    auto& frame = s->frames[s->syncContext.lockedFrame];
    image->data = frame.buffer.data();
    image->size = static_cast<int32_t>(frame.buffer.size());
    image->width = s->info.width;
    image->height = s->info.height;
    image->latency = frame.timestamp;
    image->generation = frame.generation;

    return UNECT_OK;
}

UnectResult Unect_UnlockImage(UnectSessionHandle session, UnectStreamIndex stream) {
    if (stream == UNECT_SI_BODY) 
        return UNECT_E_INVALID_ARG;

    Stream* s{};

    if (auto result = ::GetStream(session, stream, &s); result != UNECT_OK)
        return result;

    std::lock_guard<std::mutex> lock{ s->lock };

    if (s->syncContext.lockedFrame < 0)
        return UNECT_E_NOT_LOCKED;

    s->syncContext.lockedFrame = -1;

    return UNECT_OK;
}

UnectResult Unect_LockBodies(UnectSessionHandle session, UnectBodyView* body) {
    if (!body)
        return UNECT_E_INVALID_ARG;

    *body = {};
    Stream* s{};

    if (auto result = ::GetStream(session, UNECT_SI_BODY, &s); result != UNECT_OK)
        return result;

    std::lock_guard<std::mutex> lock{ s->lock };

    if (s->syncContext.lockedFrame >= 0)
        return UNECT_E_ALREADY_LOCKED;
    else if (s->syncContext.publishedFrame < 0)
        return UNECT_NO_FRAME;

    s->syncContext.lockedFrame = s->syncContext.publishedFrame;
    s->syncContext.canReplace = true;

    auto& frame = s->frames[s->syncContext.lockedFrame];
    body->bodies = std::start_lifetime_as<Body>(frame.buffer.data());
    body->bodyCount = UNECT_BODY_COUNT;
    body->floorPlane = frame.floorClipPlane;
    body->latency = frame.timestamp;
    body->generation = frame.generation;
    body->trackedCount = {};

    for (uint32_t i{}; i < UNECT_BODY_COUNT; ++i)
        if (body->bodies[i].isTracked)
            body->trackedCount++;

    return UNECT_OK;
}

UnectResult Unect_UnlockBodies(UnectSessionHandle session) {
    Stream* s{};

    if (auto result = ::GetStream(session, UNECT_SI_BODY, &s); result != UNECT_OK)
        return result;

    std::lock_guard<std::mutex> lock{ s->lock };
    
    if (s->syncContext.lockedFrame < 0)
        return UNECT_E_NOT_LOCKED;

    s->syncContext.lockedFrame = -1;
    
    return UNECT_OK;
}

#pragma endregion

#pragma region "Coordinate Mapping"

inline UnectResult GetMapper(UnectSessionHandle session, ComPtr<ICoordinateMapper>* out) {
    if (!Unect_SessionValid(session))
        return UNECT_E_STALE_SESSION;

    auto& adapter = KinectAdapter::get();

    std::lock_guard<std::mutex> lock{ adapter.lock };

    if (!adapter.coordinateMapper) 
        return UNECT_E_SENSOR_UNAVAILABLE;
    else
        *out = adapter.coordinateMapper;

    return UNECT_OK;
}

uint32_t Unect_GetMappingGeneration(UnectSessionHandle /*session*/) {
    return Internal::g_mappingGeneration.load(std::memory_order_acquire);
}

UnectResult Unect_MapDepthFrameToCameraSpace(UnectSessionHandle session, const uint16_t* data, int32_t size, UnectVector3* out, int32_t outCapacity) {
    static_assert(sizeof(CameraSpacePoint) == sizeof(UnectVector3));
    static_assert(alignof(CameraSpacePoint) == alignof(UnectVector3));

    if (!data || !out || size <= 0 || outCapacity <= 0)
        return UNECT_E_INVALID_ARG;

    ComPtr<ICoordinateMapper> mapper{};

    if (auto result = ::GetMapper(session, &mapper); result != UNECT_OK)
        return result;

    // Get the required element count.
    Stream* s{};

    if (auto result = ::GetStream(session, UNECT_SI_DEPTH, &s); result != UNECT_OK)
        return result;

    auto required = s->info.pixels();

    if (outCapacity < required) 
        return UNECT_E_BUFFER_TOO_SMALL;

    if (FAILED(mapper->MapDepthFrameToCameraSpace(static_cast<UINT>(required), data, static_cast<UINT>(required), std::start_lifetime_as<CameraSpacePoint>(out))))
        return UNECT_E_FAIL;
    
    return UNECT_OK;
}

UnectResult Unect_MapDepthFrameToColorSpace(UnectSessionHandle session, const uint16_t* data, int32_t size, UnectVector2* out, int32_t outCapacity) {
    static_assert(sizeof(ColorSpacePoint) == sizeof(UnectVector2));
    static_assert(alignof(ColorSpacePoint) == alignof(UnectVector2));

    if (!data || !out || size <= 0 || outCapacity <= 0)
        return UNECT_E_INVALID_ARG;

    ComPtr<ICoordinateMapper> mapper{};

    if (auto result = ::GetMapper(session, &mapper); result != UNECT_OK)
        return result;

    // Get the required element count.
    Stream* s{};

    if (auto result = ::GetStream(session, UNECT_SI_DEPTH, &s); result != UNECT_OK)
        return result;

    auto required = s->info.pixels();

    if (outCapacity < required)
        return UNECT_E_BUFFER_TOO_SMALL;

    if (FAILED(mapper->MapDepthFrameToColorSpace(static_cast<UINT>(required), data, static_cast<UINT>(required), std::start_lifetime_as<ColorSpacePoint>(out))))
        return UNECT_E_FAIL;

    return UNECT_OK;
}

UnectResult Unect_MapColorFrameToDepthSpace(UnectSessionHandle session, const uint16_t* data, int32_t size, UnectVector2* out, int32_t outCapacity) {
    static_assert(sizeof(DepthSpacePoint) == sizeof(UnectVector2));
    static_assert(alignof(DepthSpacePoint) == alignof(UnectVector2));

    if (!data || !out || size <= 0 || outCapacity <= 0)
        return UNECT_E_INVALID_ARG;

    ComPtr<ICoordinateMapper> mapper{};

    if (auto result = ::GetMapper(session, &mapper); result != UNECT_OK)
        return result;

    // Get the required element count.
    auto require = [&](UnectStreamIndex streamIndex, int32_t& elements) {
        Stream* s{};

        if (auto result = ::GetStream(session, streamIndex, &s); result != UNECT_OK)
            return result;

        elements = s->info.pixels();
        return UNECT_OK;
    };

    int32_t requiredDepth{}, requiredColor{};

    if (auto result = require(UNECT_SI_DEPTH, requiredDepth); result != UNECT_OK)
        return result;
    
    if (auto result = require(UNECT_SI_COLOR, requiredColor); result != UNECT_OK)
        return result;

    if (outCapacity < requiredColor)
        return UNECT_E_BUFFER_TOO_SMALL;

    if (FAILED(mapper->MapColorFrameToDepthSpace(static_cast<UINT>(requiredDepth), data, static_cast<UINT>(requiredColor), std::start_lifetime_as<DepthSpacePoint>(out))))
        return UNECT_E_FAIL;

    return UNECT_OK;
}

UnectResult Unect_MapCameraPointsToColorSpace(UnectSessionHandle session, const UnectVector3* data, int32_t size, UnectVector2* out) {
    if (!data || !out || size <= 0)
        return UNECT_E_INVALID_ARG;

    ComPtr<ICoordinateMapper> mapper{};

    if (auto result = ::GetMapper(session, &mapper); result != UNECT_OK)
        return result;

    if (FAILED(mapper->MapCameraPointsToColorSpace(static_cast<UINT>(size), std::start_lifetime_as<const CameraSpacePoint>(data), static_cast<UINT>(size), std::start_lifetime_as<ColorSpacePoint>(out))))
        return UNECT_E_FAIL;

    return UNECT_OK;
}

UnectResult Unect_MapCameraPointsToDepthSpace(UnectSessionHandle session, const UnectVector3* data, int32_t size, UnectVector2* out) {
    if (!data || !out || size <= 0)
        return UNECT_E_INVALID_ARG;

    ComPtr<ICoordinateMapper> mapper{};

    if (auto result = ::GetMapper(session, &mapper); result != UNECT_OK)
        return result;

    if (FAILED(mapper->MapCameraPointsToDepthSpace(static_cast<UINT>(size), std::start_lifetime_as<const CameraSpacePoint>(data), static_cast<UINT>(size), std::start_lifetime_as<DepthSpacePoint>(out))))
        return UNECT_E_FAIL;

    return UNECT_OK;
}

#pragma endregion