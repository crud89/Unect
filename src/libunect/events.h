#pragma once

#include <libunect.h>

#include <cstdint>

/// <summary>
/// The type of an event pushed to <see cref="PushEvent" />
/// </summary>
typedef enum UnectEventType : uint32_t {

    /// <summary>
    /// Indicates that the sensor state has changed.
    /// </summary>
    UNECT_EVENT_SENSOR_STATE = 1,

    /// <summary>
    /// Indicates that a new body is tracked.
    /// </summary>
    UNECT_EVENT_BODY_ENTERED = 2,

    /// <summary>
    /// Indicates that a body is no longer tracked.
    /// </summary>
    UNECT_EVENT_BODY_LEFT = 3,

    /// <summary>
    /// Indicates that the coordinate mapping has changed.
    /// </summary>
    UNECT_EVENT_MAPPING_CHANGED = 4,

    /// <summary>
    /// Indicates that a stream error has occurred.
    /// </summary>
    UNECT_EVENT_STREAM_ERROR = 5

} UnectEventType;

/// <summary>
/// Pushes a new event to the events cache.
/// </summary>
/// <param name="type"></param>
/// <param name="param1">The first parameter for the event.</param>
/// <param name="param2">The second parameter for the event.</param>
void PushEvent(UnectEventType type, int32_t param1, uint64_t param2);

/// <summary>
/// Resets the cached events.
/// </summary>
void ResetEvents();

/// <summary>
/// Returns all cached events.
/// </summary>
/// <param name="eventBuffer">A pointer to the event buffer.</param>
/// <param name="capacity">The capacity of the event buffer.</param>
/// <param name="eventCount">The number of events actually written to the event buffer.</param>
/// <param name="dropCount">The number of events dropped from the cache since the last call.</param>
/// <returns>The return code of the function.</returns>
UnectResult PopEvents(UnectEventType* eventBuffer, int32_t capacity, int32_t* eventCount, int32_t* dropCount);