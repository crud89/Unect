#pragma once

#include <libunect.h>

#include "kinect.h"

#include <mutex>
#include <unordered_set>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

/// <summary>
/// Represents the registry that stores the session handles.
/// </summary>
struct Registry {

    Registry(const Registry&) = delete;
    Registry(Registry&&) noexcept = delete;
    Registry& operator=(const Registry&) = delete;
    Registry& operator=(Registry&&) noexcept = delete;

    /// <summary>
    /// A mutex that is used to lock access to the registry.
    /// </summary>
    std::mutex lock{};

    /// <summary>
    /// Stores the next session handle.
    /// </summary>
    uint32_t nextHandleId{ 1 };

    /// <summary>
    /// Stores the living session handles.
    /// </summary>
    std::unordered_set<uint32_t> liveHandles{};

    /// <summary>
    /// Stores the descriptor for the current sessions.
    /// </summary>
    UnectSessionDesc sessionDesc{};

    /// <summary>
    /// Stores the flag that indicates if the streams are active.
    /// </summary>
    bool streamsRunning{ false };

    /// <summary>
    /// Stores the pointer to the sensor.
    /// </summary>
    ComPtr<IKinectSensor> sensor{};

    /// <summary>
    /// Returns the registry singleton.
    /// </summary>
    /// <returns>A reference to the session registry.</returns>
    static Registry& get();

    /// <summary>
    /// Attempts to open the sensor.
    /// </summary>
    /// <returns>The return code of the function.</returns>
    UnectResult OpenSensor();

    /// <summary>
    /// Allocates the next handle and returns it.
    /// </summary>
    /// <returns>The next handle to use.</returns>
    uint32_t AllocNextHandle();

    /// <summary>
    /// Validates a session handle
    /// </summary>
    /// <param name="session">The session handle to validate</param>
    /// <returns>`true` if the session is a valid handle from the current epoch and `false` otherwise.</returns>
    bool IsValidHandle(UnectSessionHandle session);

    /// <summary>
    /// Releases a session handle.
    /// </summary>
    /// <param name="session">The session handle to release.</param>
    /// <returns>The return code of the function.</returns>
    UnectResult ReleaseSession(UnectSessionHandle session);

private:
    /// <summary>
    /// Ends the current epoch.
    /// </summary>
    void EndEpoch();

private:
    /// <summary>
    /// Creates a new registry instance.
    /// </summary>
    /// <seealso cref="Registry::get" />
    Registry() = default;

    /// <summary>
    /// The purposefully deleted destructor of the registry.
    /// </summary>
    ~Registry() = delete;

};