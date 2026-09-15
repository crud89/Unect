#pragma once

#include <libunect/libunect_export.h>

#include "vector.h"
#include "body.h"

#include <stdint.h>

#if defined(_WIN32)
#  define UNECT_CALL __stdcall
#else 
#  define UNECT_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /// <summary>
    /// Stores the ABI version (used during development to check if a library should be reloaded).
    /// </summary>
    constexpr static int32_t UNECT_ABI_VERSION = 1;

    /// <summary>
    /// Returns the current ABI version.
    /// </summary>
    /// <returns>The current ABI version.</returns>
    constexpr LIBUNECT_EXPORT int32_t UNECT_CALL Unect_GetAbiVersion() {
        return UNECT_ABI_VERSION;
    }

#pragma region "Enumerations"

    /// <summary>
    /// A collection of valid error codes.
    /// </summary>
    typedef enum UnectResult : int32_t {

        /// <summary>
        /// No error occurred.
        /// </summary>
        UNECT_OK                    = 0,

        /// <summary>
        /// There's currently no new frame available.
        /// </summary>
        UNECT_NO_FRAME              = 1,

        /// <summary>
        /// The requested operation failed.
        /// </summary>
        UNECT_E_FAIL                = -1,

        /// <summary>
        /// An invalid argument has been provided.
        /// </summary>
        UNECT_E_INVALID_ARG         = -2,

        /// <summary>
        /// An handle has outlived its generation/epoch.
        /// </summary>
        UNECT_E_STALE_SESSION       = -3,

        /// <summary>
        /// The requested stream is not enabled.
        /// </summary>
        UNECT_E_STREAM_NOT_ENABLED  = -4,

        /// <summary>
        /// The provided buffer is too small.
        /// </summary>
        UNECT_E_BUFFER_TOO_SMALL    = -5,

        /// <summary>
        /// The sensor is unavailable.
        /// </summary>
        UNECT_E_SENSOR_UNAVAILABLE  = -6,

        /// <summary>
        /// A resource is already locked.
        /// </summary>
        UNECT_E_ALREADY_LOCKED      = -7,

        /// <summary>
        /// A resource is not locked.
        /// </summary>
        UNECT_E_NOT_LOCKED          = -8,

        /// <summary>
        /// There has been an ABI mismatch.
        /// </summary>
        UNECT_E_ABI_MISMATCH        = -9,

        /// <summary>
        /// The requested operation is unsupported.
        /// </summary>
        UNECT_E_UNSUPPORTED         = -10,

        /// <summary>
        /// The requested operation is currently not implemented.
        /// </summary>
        UNECT_E_NOT_IMPLEMENTED     = -11

    } UnectResult;

    /// <summary>
    /// Represents a type of a data stream.
    /// </summary>
    typedef enum UnectStreamType : uint32_t {

        /// <summary>
        /// Corresponds to no data stream.
        /// </summary>
        UNECT_STREAM_NONE              = 0,

        /// <summary>
        /// Corresponds to the depth image stream.
        /// </summary>
        UNECT_STREAM_DEPTH             = 1 << 0,

        /// <summary>
        /// Corresponds to the color image stream.
        /// </summary>
        UNECT_STREAM_COLOR             = 1 << 1,

        /// <summary>
        /// Corresponds to the IR image stream.
        /// </summary>
        UNECT_STREAM_INFRARED          = 1 << 2,

        /// <summary>
        /// Corresponds to the long exposure IR image stream.
        /// </summary>
        UNECT_STREAM_LONG_EXPOSURE_IR  = 1 << 3,

        /// <summary>
        /// Corresponds to the body index image stream.
        /// </summary>
        UNECT_STREAM_BODY_INDEX        = 1 << 4,

        /// <summary>
        /// Corresponds to the body stream.
        /// </summary>
        UNECT_STREAM_BODY              = 1 << 5,

        /// <summary>
        /// Corresponds to all streams.
        /// </summary>
        UNECT_STREAM_ALL               = 0x3F

    } UnectStreamType;

    /// <summary>
    /// Maps indices to individual streams.
    /// </summary>
    typedef enum UnectStreamIndex : int32_t {

        /// <summary>
        /// Corresponds to the index of the depth image stream.
        /// </summary>
        UNECT_SI_DEPTH = 0,

        /// <summary>
        /// Corresponds to the index of the color image stream.
        /// </summary>
        UNECT_SI_COLOR,

        /// <summary>
        /// Corresponds to the index of the IR image stream.
        /// </summary>
        UNECT_SI_INFRARED,

        /// <summary>
        /// Corresponds to the index of the long exposure IR image stream.
        /// </summary>
        UNECT_SI_LONG_EXPOSURE_IR,

        /// <summary>
        /// Corresponds to the index of the body index image stream.
        /// </summary>
        UNECT_SI_BODY_INDEX,

        /// <summary>
        /// Corresponds to the index of the body stream.
        /// </summary>
        UNECT_SI_BODY,

        /// <summary>
        /// Stores the number of available streams.
        /// </summary>
        UNECT_SI_COUNT

    } UnectStreamIndex;

    /// <summary>
    /// Represents supported color formats.
    /// </summary>
    typedef enum UnectColorFormat : int32_t {

        /// <summary>
        /// Outputs colors in BGRA order with 32 bit depth.
        /// </summary>
        UNECT_COLOR_BGRA32              = 0,

        /// <summary>
        /// Outputs colors in RGBA order with 32 bit color depth.
        /// </summary>
        UNECT_COLOR_RGBA32              = 1,

        /// <summary>
        /// A native color format without conversion cost.
        /// </summary>
        UNECT_COLOR_YUY2                = 2

    } UnectColorFormat;

    /// <summary>
    /// Represents the states of a sensor.
    /// </summary>
    typedef enum UnectSensorState : uint32_t {

        /// <summary>
        /// The sensor is currently closed.
        /// </summary>
        UNECT_SENSOR_CLOSED = 0, 

        /// <summary>
        /// The sensor is currently opening.
        /// </summary>
        UNECT_SENSOR_OPENING, 

        /// <summary>
        /// The sensor is available.
        /// </summary>
        UNECT_SENSOR_AVAILABLE, 

        /// <summary>
        /// The sensor is not available.
        /// </summary>
        UNECT_SENSOR_UNAVAILABLE

    } UnectSensorState;

#pragma endregion

#pragma region "Session"

    /// <summary>
    /// The type used to identify a session.
    /// </summary>
    typedef uint64_t UnectSessionHandle;

    /// <summary>
    /// Represents an invalid session.
    /// </summary>
    constexpr static UnectSessionHandle UNECT_INVALID_SESSION_HANDLE = 0ull;

    /// <summary>
    /// Represents flags that control the session behavior.
    /// </summary>
    typedef enum UnectSessionFlags : uint32_t {
        
        /// <summary>
        /// Corresponds to a default session behavior.
        /// </summary>
        UNECT_SESSION_NONE = 0,
        
        /// <summary>
        /// Keeps the session alive, even after all references are released through <see cref="UnectReleaseSession" />.
        /// </summary>
        UNECT_SESSION_KEEP_ALIVE = 1 << 0

    } UnectSessionFlags;

    /// <summary>
    /// The minimum number of back buffers.
    /// </summary>
    constexpr uint32_t UNECT_MIN_BUFFERS = 2;

    /// <summary>
    /// The maximum number of back buffers.
    /// </summary>
    constexpr uint32_t UNECT_MAX_BUFFERS = 8;

    /// <summary>
    /// Describes a session.
    /// </summary>
    typedef struct UnectSessionDesc {
        
        /// <summary>
        /// Stores a mask of available streams.
        /// </summary>
        UnectStreamType streams{ UNECT_STREAM_ALL };
        
        /// <summary>
        /// Stores the format of the color stream. 
        /// </summary>
        UnectColorFormat colorFormat{ UNECT_COLOR_YUY2 };
        
        /// <summary>
        /// Stores the number of buffers used by the session.
        /// </summary>
        /// <seealso cref="UNECT_MIN_BUFFERS" />
        /// <seealso cref="UNECT_MAX_BUFFERS" />
        int32_t bufferCount{ 3 };
        
        /// <summary>
        /// Stores the session flags that control its behavior.
        /// </summary>
        UnectSessionFlags flags{ UNECT_SESSION_NONE };

    } UnectSessionDesc;

    /// <summary>
    /// Acquires a new session handle.
    /// </summary>
    /// <remarks>
    /// Repeated calls with a compatible <paramref name="sessionDesc" /> return the same session pointer. If a subsequent call asks for additional streams, the underlying 
    /// session will be extended accordingly.
    /// 
    /// Sessions must be released by calling <see cref="Unect_ReleaseSession" />.
    /// </remarks>
    /// <param name="sessionDesc">The session descriptor used to determine the session state.</param>
    /// <param name="session">The pointer to the session handle.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_GetSession(const UnectSessionDesc* sessionDesc, UnectSessionHandle* session);

    /// <summary>
    /// Releases a session handle.
    /// </summary>
    /// <param name="session">The session handle to release.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_ReleaseSession(UnectSessionHandle session);

    /// <summary>
    /// Verifies if a session is valid.
    /// </summary>
    /// <param name="session">The session to verify.</param>
    /// <returns>`true` if the session is valid and `false` otherwise.</returns>
    LIBUNECT_EXPORT bool UNECT_CALL Unect_SessionValid(UnectSessionHandle session);

    /// <summary>
    /// Unconditionally shuts down the library.
    /// </summary>
    LIBUNECT_EXPORT void UNECT_CALL Unect_Shutdown();

    /// <summary>
    /// Returns the epoch of the library runtime.
    /// </summary>
    /// <remarks>
    /// An epoch is incremented whenever all sessions are released. This signals to callers that resources to this library should be rebound.
    /// </remarks>
    /// <returns>The epoch of the library runtime.</returns>
    LIBUNECT_EXPORT uint32_t UNECT_CALL Unect_Epoch();

    /// <summary>
    /// Retrieves the state of the connected sensor.
    /// </summary>
    /// <param name="session">The session handle on which to query for the sensor.</param>
    /// <param name="state">A pointer to the state variable.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_GetSensorState(UnectSessionHandle session, UnectSensorState* state);

#pragma endregion

#pragma region "Streaming"
    
    /// <summary>
    /// Stores meta-data about a stream.
    /// </summary>
    typedef struct UnectStreamInfo {

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
        /// Stores the number of pixels.
        /// </summary>
        int32_t pixelCount{};

        /// <summary>
        /// Stores the overall size of a frame (in bytes).
        /// </summary>
        uint32_t totalSize{};

    } UnectStreamInfo;

    /// <summary>
    /// Represents a view over an image from a stream.
    /// </summary>
    typedef struct UnectImageView {

        /// <summary>
        /// A pointer to the image pixels.
        /// </summary>
        const void* data{};

        /// <summary>
        /// The size of the image in bytes.
        /// </summary>
        int32_t size{};

        /// <summary>
        /// The width of the image in pixels.
        /// </summary>
        int32_t width{};

        /// <summary>
        /// The height of the image in pixels.
        /// </summary>
        int32_t height{};

        /// <summary>
        /// The latency at which the image has been acquired in milliseconds.
        /// </summary>
        int64_t latency{};

        /// <summary>
        /// The generation of the image.
        /// </summary>
        uint64_t generation{};

    } UnectImageView;

    /// <summary>
    /// Represents a view over a body from a stream.
    /// </summary>
    typedef struct UnectBodyView {

        /// <summary>
        /// A pointer to the bodies array, contains <see cref="bodyCount" /> elements.
        /// </summary>
        const Body* bodies{};

        /// <summary>
        /// The number of bodies in the <see cref="bodies" /> array.
        /// </summary>
        int32_t bodyCount{};

        /// <summary>
        /// The number of bodies in the <see cref="bodies" /> array that are currently tracked.
        /// </summary>
        int32_t trackedCount{};

        /// <summary>
        /// The floor clip plane in parametric form.
        /// </summary>
        UnectVector4 floorPlane{};

        /// <summary>
        /// The latency at which the image has been acquired in milliseconds.
        /// </summary>
        int64_t latency{};

        /// <summary>
        /// The generation of the image.
        /// </summary>
        uint64_t generation{};

    } UnectBodyView;

    /// <summary>
    /// Returns the current mapping generation, that is incremented, if the underlying sensor changes the coordinate mapping reference.
    /// </summary>
    /// <param name="session">The session from which to obtain the mapping generation.</param>
    /// <returns>The current mapping generation.</returns>
    //LIBUNECT_EXPORT uint32_t UNECT_CALL Unect_GetMappingGeneration(UnectSessionHandle session);

    //LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_MapDepthFrameToCameraSpace(UnectSessionHandle session, const uint16_t* depth, int32_t depthCount, UnectVector3* out, int32_t outCapacity);

    //LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_MapDepthFrameToColorSpace(UnectSessionHandle session, const uint16_t* depth, int32_t depthCount, UnectVector2* out, int32_t outCapacity);

    //LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_MapColorFrameToDepthSpace(UnectSessionHandle session, const uint16_t* depth, int32_t depthCount, UnectVector2* out, int32_t outCapacity);

    //LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_MapCameraPointsToColorSpace(UnectSessionHandle session, const UnectVector3* pts, int32_t count, UnectVector2* out);

    //LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_MapCameraPointsToDepthSpace(UnectSessionHandle session, const UnectVector3* pts, int32_t count, UnectVector2* out);

    /// <summary>
    /// Returns meta-data about a specific stream.
    /// </summary>
    /// <param name="session">The session from which to acquire the stream.</param>
    /// <param name="stream">The stream to query.</param>
    /// <param name="info">A pointer to the stream info.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_GetStreamInfo(UnectSessionHandle session, UnectStreamIndex stream, UnectStreamInfo* info);

    /// <summary>
    /// Returns the generation of an image stream.
    /// </summary>
    /// <param name="session">The session on which to query the stream.</param>
    /// <param name="stream">The index of the stream.</param>
    /// <returns>The image generation on the stream.</returns>
    LIBUNECT_EXPORT uint64_t UNECT_CALL Kinect2_PeekGeneration(UnectSessionHandle session, UnectStreamIndex stream);

    /// <summary>
    /// Acquires an image from the image stream.
    /// </summary>
    /// <param name="session">The session from which to acquire the image.</param>
    /// <param name="stream">The stream from which to obtain the image.</param>
    /// <param name="image">A pointer to the image view.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_LockImage(UnectSessionHandle session, UnectStreamIndex stream, UnectImageView* image);

    /// <summary>
    /// Releases image back to the stream.
    /// </summary>
    /// <param name="session">The session from which the image was acquired.</param>
    /// <param name="stream">The stream from which the image was obtained.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_UnlockImage(UnectSessionHandle session, UnectStreamIndex stream);

    /// <summary>
    /// Acquires a body from the bodies stream.
    /// </summary>
    /// <param name="session">The session from which to acquire the body.</param>
    /// <param name="body">A pointer to the body view.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_LockBodies(UnectSessionHandle session, UnectBodyView* body);

    /// <summary>
    /// Releases a body back to the stream.
    /// </summary>
    /// <param name="session">The session from which the image was acquired.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_UnlockBodies(UnectSessionHandle session);

#pragma endregion

#pragma region "Diagnostics"

    /// <summary>
    /// Stores diagnostics reported from the library.
    /// </summary>
    typedef struct UnectStreamStats {

        /// <summary>
        /// The number of arrived frames.
        /// </summary>
        uint64_t framesArrived;

        /// <summary>
        /// The number of dropped frames.
        /// </summary>
        uint64_t framesDropped;

        /// <summary>
        /// The frame-rate in frames per second.
        /// </summary>
        float framerate;

        /// <summary>
        /// The last frame's latency in milliseconds.
        /// </summary>
        float lastLatency;

    } UnectStreamStats;

    /// <summary>
    /// Returns the current diagnostics for a particular stream.
    /// </summary>
    /// <param name="session">The session that contains the stream.</param>
    /// <param name="stream">The stream about which to obtain the diagnostics.</param>
    /// <param name="stats">A pointer to the diagnostics container.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_GetStreamStats(UnectSessionHandle session, UnectStreamIndex stream, UnectStreamStats* stats);

    /// <summary>
    /// Returns a human-readable representation of an error code.
    /// </summary>
    /// <param name="result">The error (or result) code.</param>
    /// <returns>A human-readable representation of the result.</returns>
    LIBUNECT_EXPORT const char* UNECT_CALL Unect_GetResultString(UnectResult result);

    /// <summary>
    /// Reports the cached log messages.
    /// </summary>
    /// <param name="buffer">The buffer into which to copy the log messages.</param>
    /// <param name="capacity">The capacity of <paramref name="buffer"/>.</param>
    /// <param name="outBytes">The number of bytes actually copied into <paramref name="buffer" />.</param>
    /// <returns>The return code of the function.</returns>
    LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_GetLog(char* buffer, int32_t capacity, int32_t* outBytes);

#pragma endregion

#ifdef __cplusplus
} // extern "C"
#endif