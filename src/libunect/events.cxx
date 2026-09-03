#include "events.h"

#include <atomic>
#include <chrono>
#include <cstddef>

using Clock = std::chrono::steady_clock;

#pragma region "Public interface"

void PushEvent(UnectEventType type, int32_t i32, uint64_t u64) {
    throw;
}

void ResetEvents() {
    throw;
}

UnectResult PopEvents(UnectEventType* eventBuffer, int32_t capacity, int32_t* eventCount, int32_t* dropCount) {
    throw;
}

#pragma endregion