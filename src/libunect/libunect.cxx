#include <libunect.h>

#include "wrapper/kinect.h"
#include "helpers.hpp"
#include "registry.h"
#include "internal.h"

#include <algorithm>
#include <mutex>

namespace Internal { 

    std::atomic<int32_t> g_sensorState{ UNECT_SENSOR_CLOSED }; 

    std::atomic<uint32_t> g_epoch{ 0u };

    std::atomic<uint32_t> g_mappingGeneration{ 0 };

}

#pragma region "Session"

UnectResult Unect_GetSession(const UnectSessionDesc* sessionDesc, UnectSessionHandle* session) {
    // Validate input arguments.
    if (!session)
        return UNECT_E_INVALID_ARG;
    
    *session = UNECT_INVALID_SESSION_HANDLE;

    if (!sessionDesc)
        return UNECT_E_INVALID_ARG;

    if (sessionDesc->streams == UNECT_STREAM_NONE)
        return UNECT_E_INVALID_ARG;

    if (sessionDesc->streams & ~static_cast<uint32_t>(UNECT_STREAM_ALL))
        return UNECT_E_INVALID_ARG;

    if (!ValidColorFormat(sessionDesc->colorFormat))
        return UNECT_E_INVALID_ARG;

    if (sessionDesc->bufferCount < UNECT_MIN_BUFFERS || sessionDesc->bufferCount > UNECT_MAX_BUFFERS)
        return UNECT_E_INVALID_ARG;

    // Lock the registry.
    auto& registry = Registry::get();
    std::lock_guard<std::mutex> lock{ registry.lock };

    // Check if there are any live handles. If not, this is the first call to acquire in this epoch.
    if (registry.liveHandles.empty())
    {
        // Open the sensor.
        if (auto result = registry.OpenSensor(); result != UNECT_OK)
            return result;

        // Store the session desc.
        registry.sessionDesc = *sessionDesc;

        // Start streaming.
        if (auto result = StartStreaming(registry.sensor.Get(), registry.sessionDesc); result != UNECT_OK)
        {
            // ... it didn't work. Check if the sensor should remain open and handle cleanup.
            if (!(registry.sessionDesc.flags & UNECT_SESSION_KEEP_ALIVE) && registry.sensor) 
            {
                registry.sensor->Close();
                registry.sensor.Reset();
                Internal::g_sensorState.store(UNECT_SENSOR_CLOSED, std::memory_order_relaxed);
            }

            registry.sessionDesc.streams = UNECT_STREAM_NONE;
            registry.sessionDesc.flags = UNECT_SESSION_NONE;

            return result;
        }

        registry.streamsRunning = true;
    }
    else
    {
        // Check for color format mismatches. 
        if (registry.sessionDesc.colorFormat != sessionDesc->colorFormat)
            return UNECT_E_INVALID_ARG;

        // Widen streams.
        UnectStreamType missingStreams = static_cast<UnectStreamType>(sessionDesc->streams & ~registry.sessionDesc.streams);

        if (missingStreams != 0u) {
            if (auto result = WidenStreams(registry.sensor.Get(), missingStreams); result != UNECT_OK)
                return result;
            else
                registry.sessionDesc.streams = static_cast<UnectStreamType>(std::to_underlying(registry.sessionDesc.streams) | std::to_underlying(missingStreams));
        }

        // Store the flags. This can enable the `KEEP_ALIVE` flag after initial acquisition without it.
        registry.sessionDesc.flags = static_cast<UnectSessionFlags>(std::to_underlying(registry.sessionDesc.flags) | std::to_underlying(sessionDesc->flags));
    }

    // Create the handle and return.
    *session = MakeHandle(Internal::g_epoch.load(std::memory_order_relaxed), registry.AllocNextHandle());
    return UNECT_OK;
}

UnectResult Unect_ReleaseSession(UnectSessionHandle session) {
    auto& registry = Registry::get();
    std::lock_guard<std::mutex> lock{ registry.lock };

    return registry.ReleaseSession(session);
}

bool Unect_SessionValid(UnectSessionHandle session) {
    auto& registry = Registry::get();
    std::lock_guard<std::mutex> lock{ registry.lock };

    return registry.IsValidHandle(session);
}

void UNECT_CALL Unect_Shutdown() {
    auto& registry = Registry::get();
    std::lock_guard<std::mutex> lock{ registry.lock };

    // Check if there are session alive.
    auto wasAlive = !registry.liveHandles.empty();

    // Close all streams.
    if (registry.streamsRunning) {
        ForceUnlockStreams();
        StopStreams();
        registry.streamsRunning = false;
    }

    // Release the sensor.
    if (registry.sensor) {
        registry.sensor->Close();
        registry.sensor.Reset();
    }

    Internal::g_sensorState.store(UNECT_SENSOR_CLOSED, std::memory_order_relaxed);

    // Clear the registry.
    registry.liveHandles.clear();
    registry.sessionDesc.streams = UNECT_STREAM_NONE;
    registry.sessionDesc.flags = UNECT_SESSION_NONE;
    registry.sessionDesc.bufferCount = 3;

    // Increment epoch if there are open handles remaining.
    if (wasAlive)
        Internal::g_epoch.fetch_add(1, std::memory_order_release);
}

uint32_t Unect_Epoch() {
    return Internal::g_epoch.load(std::memory_order_acquire);
}

UnectResult Unect_GetSensorState(UnectSessionHandle session, UnectSensorState* state) {
    if (!state) 
        return UNECT_E_INVALID_ARG;

    *state = UNECT_SENSOR_CLOSED;

    auto& registry = Registry::get();
    std::lock_guard<std::mutex> lock{ registry.lock };

    if (!registry.IsValidHandle(session)) 
        return UNECT_E_STALE_SESSION;

    *state = static_cast<UnectSensorState>(Internal::g_sensorState.load(std::memory_order_relaxed));

    return UNECT_OK;
}

#pragma endregion

#pragma region "Diagnostics"

const char* Unect_GetResultString(UnectResult result) {
    switch (result) {
    case UNECT_OK:
        return "UNECT_OK";
    case UNECT_NO_FRAME:
        return "UNECT_NO_FRAME";
    case UNECT_E_FAIL:
        return "UNECT_E_FAIL";
    case UNECT_E_INVALID_ARG:
        return "UNECT_E_INVALID_ARG";
    case UNECT_E_STALE_SESSION:
        return "UNECT_E_STALE_SESSION";
    case UNECT_E_STREAM_NOT_ENABLED:
        return "UNECT_E_STREAM_NOT_ENABLED";
    case UNECT_E_BUFFER_TOO_SMALL:
        return "UNECT_E_BUFFER_TOO_SMALL";
    case UNECT_E_SENSOR_UNAVAILABLE:
        return "UNECT_E_SENSOR_UNAVAILABLE";
    case UNECT_E_ALREADY_LOCKED:
        return "UNECT_E_ALREADY_LOCKED";
    case UNECT_E_NOT_LOCKED:
        return "UNECT_E_NOT_LOCKED";
    case UNECT_E_ABI_MISMATCH:
        return "UNECT_E_ABI_MISMATCH";
    case UNECT_E_UNSUPPORTED:
        return "UNECT_E_UNSUPPORTED";
    case UNECT_E_NOT_IMPLEMENTED:
        return "UNECT_E_NOT_IMPLEMENTED";
    default:
        return "UNKNOWN";
    }
}

UnectResult Unect_GetLog(char* buffer, int32_t capacity, int32_t* outBytes) {
    return UNECT_E_NOT_IMPLEMENTED;
}

#pragma endregion