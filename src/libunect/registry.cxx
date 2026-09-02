#include "registry.h"
#include "internal.h"

#include "helpers.hpp"

#include <cstddef>
#include <new>

Registry& Registry::get() {
    // Placement new is allocating the registry in static memory, so it doesn't "leak".
    alignas(Registry) static std::byte storage[sizeof(Registry)];
    static Registry* r = new (storage) Registry();

    return *r;
}

UnectResult Registry::OpenSensor() {
    // Check if it's already open.
    if (sensor)
        return UNECT_OK;

    // Attempt to open the sensor.
    ComPtr<IKinectSensor> s;
    
    if (auto hr = ::GetDefaultKinectSensor(&s); FAILED(hr) || !s)
        return UNECT_E_SENSOR_UNAVAILABLE;

    if (auto hr = s->Open(); FAILED(hr))
        return UNECT_E_SENSOR_UNAVAILABLE;

    // Store the sensor.
    sensor = s;
    Internal::g_sensorState.store(UNECT_SENSOR_OPENING, std::memory_order_relaxed);

    return UNECT_OK;
}

uint32_t Registry::AllocNextHandle() {
    // Prevent zero (invalid) session handle on overflows.
    nextHandleId = std::max(1u, nextHandleId);

    auto handle = nextHandleId++;
    liveHandles.insert(handle);

    return handle;
}

bool Registry::IsValidHandle(UnectSessionHandle session) {
    if (session == UNECT_INVALID_SESSION_HANDLE) 
        return false;
    
    if (HandleGeneration(session) != Internal::g_epoch.load(std::memory_order_relaxed))
        return false;

    return liveHandles.find(HandleIdentifier(session)) != liveHandles.end();
}

UnectResult Registry::ReleaseSession(UnectSessionHandle session) {
    if (!IsValidHandle(session))
        return UNECT_E_STALE_SESSION;

    liveHandles.erase(HandleIdentifier(session));

    if (liveHandles.empty()) 
        EndEpoch();

    return UNECT_OK;
}

void Registry::EndEpoch() {
    // Stop streaming.
    if (streamsRunning) {
        ForceUnlockStreams();
        StopStreams();
        streamsRunning = false;
    }

    auto keepAlive = (sessionDesc.flags & UNECT_SESSION_KEEP_ALIVE) != 0;
    
    // Close the sensor if the flags aren't configured otherwise.
    if (!keepAlive && sensor) {
        sensor->Close();
        sensor.Reset();

        Internal::g_sensorState.store(UNECT_SENSOR_CLOSED, std::memory_order_relaxed);
    }

    // Reset the session descriptor.
    sessionDesc.streams = UNECT_STREAM_NONE;
    sessionDesc.flags = UNECT_SESSION_NONE;
    sessionDesc.bufferCount = 3;

    // Increment the current epoch counter.
    Internal::g_epoch.fetch_add(1, std::memory_order_release);
}