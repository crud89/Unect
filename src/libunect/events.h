#pragma once

#include <libunect.h>

#include <cstdint>

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
/// <param name="events">A pointer to the event buffer.</param>
/// <param name="capacity">The capacity of the event buffer.</param>
/// <param name="eventCount">The number of events actually written to the event buffer.</param>
/// <param name="dropCount">The number of events dropped from the cache since the last call.</param>
/// <returns>The return code of the function.</returns>
UnectResult PopEvents(UnectEvent* events, int32_t capacity, int32_t* eventCount, int32_t* dropCount);