#include "events.h"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <array>

using Clock = std::chrono::steady_clock;

/// <summary>
/// Checks if <paramref name="val" /> is a power-of-two.
/// </summary>
/// <param name="val">The value to check.</param>
/// <returns>`true` if the value is a power-of-two and `false` otherwise.</returns>
constexpr bool isPoT(size_t val) {
    return val && ((val & (val - 1)) == 0);
}

/// <summary>
/// Stores the size of a cache line.
/// </summary>
constexpr size_t CACHE_LINE_SIZE =
#ifdef __cpp_lib_hardware_interference_size
    std::hardware_destructive_interference_size;
#else
    64u;
#endif

#pragma region "Event Buffer"

/// <summary>
/// The capacity of the event buffer. Must be power-of-two.
/// </summary>
constexpr size_t EVENT_BUFFER_CAPACITY = 256;

static_assert(isPoT(EVENT_BUFFER_CAPACITY), "The event buffer capacity must be a power of two.");

/// <summary>
/// Stores an event in the event buffer's event queue.
/// </summary>
struct EventBufferEntry final {

    /// <summary>
    /// Stores the event sequence.
    /// </summary>
    std::atomic<size_t> sequence{};

    /// <summary>
    /// Stores the event.
    /// </summary>
    UnectEvent event{};

};

/// <summary>
/// A ring-buffer structure for efficiently storing an event queue.
/// </summary>
struct EventBuffer final {

    /// <summary>
    /// Stores the event queue.
    /// </summary>
    std::array<EventBufferEntry, EVENT_BUFFER_CAPACITY> events{};

    /// <summary>
    /// Stores the event buffer's writer position.
    /// </summary>
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> writerPosition{};

    /// <summary>
    /// Stores the event buffer's reader position.
    /// </summary>
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> readerPosition{};

    /// <summary>
    /// Stores the number of dropped events.
    /// </summary>
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> dropped{};

    /// <summary>
    /// Returns a reference to the event buffer singleton instance.
    /// </summary>
    /// <returns>A reference to the event buffer singleton instance.</returns>
    static EventBuffer& get() {
        alignas(EventBuffer) static std::byte storage[sizeof(EventBuffer)];
        static EventBuffer* b = new (storage) EventBuffer();

        return *b;
    }

private:
    /// <summary>
    /// Creates a new event buffer instance.
    /// </summary>
    EventBuffer() { 
        for (size_t i{}; i < EVENT_BUFFER_CAPACITY; ++i) 
            events[i].sequence.store(i, std::memory_order_relaxed);
    }

    /// <summary>
    /// The purposefully deleted destructor of the event buffer.
    /// </summary>
    ~EventBuffer() = delete;
};

/// <summary>
/// Returns the current timestamp relative to the first call of this function in milliseconds.
/// </summary>
/// <returns>The current timestamp in milliseconds.</returns>
int64_t getNow() {
    static const Clock::time_point origin = Clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - origin).count();
}

#pragma endregion

#pragma region "Public interface"

void PushEvent(UnectEventType type, int32_t i32, uint64_t u64) {
    auto& eventBuffer = EventBuffer::get();

    size_t pos = eventBuffer.writerPosition.load(std::memory_order_relaxed);
    EventBufferEntry* entry{};

    // Find a free entry in the event queue.
    while (true) {
        // Get the entry and check if it is already claimed.
        entry = &eventBuffer.events[pos & (EVENT_BUFFER_CAPACITY - 1)];
        const size_t seq = entry->sequence.load(std::memory_order_acquire);
        const intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

        if (diff == 0) {
            // Claim the position.
            if (eventBuffer.writerPosition.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                break;
        }
        else if (diff < 0) {
            // No more slots available, so we need to drop the event.
            eventBuffer.dropped.fetch_add(1, std::memory_order_relaxed);
            return;
        }
        else {
            pos = eventBuffer.writerPosition.load(std::memory_order_relaxed);
        }
    }

    entry->event.type = type;
    entry->event.param1 = i32;
    entry->event.param2 = u64;
    entry->event.timestamp = getNow();
    entry->sequence.store(pos + 1, std::memory_order_release);
}

void ResetEvents() {
    auto& eventBuffer = EventBuffer::get();
    std::array<UnectEvent, EVENT_BUFFER_CAPACITY> events{};
    int32_t count{}, dropped{};
    PopEvents(events.data(), static_cast<int32_t>(events.size()), &count, &dropped);
    eventBuffer.dropped.store(0, std::memory_order_relaxed);
}

UnectResult PopEvents(UnectEvent* events, int32_t capacity, int32_t* eventCount, int32_t* dropCount) {
    if (!events || !*eventCount || !dropCount || capacity <= 0)
        return UNECT_E_INVALID_ARG;

    auto& eventBuffer = EventBuffer::get();
    size_t n{}, pos{ eventBuffer.readerPosition.load(std::memory_order_relaxed) };

    while (n < capacity) {
        auto event = &eventBuffer.events[pos & (EVENT_BUFFER_CAPACITY - 1)];

        const size_t seq = event->sequence.load(std::memory_order_acquire);
        const intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

        // If there are no more events, return.
        if (diff != 0) 
            break;

        events[n++] = event->event;
        event->sequence.store(pos + EVENT_BUFFER_CAPACITY, std::memory_order_release);
        ++pos;

        eventBuffer.readerPosition.store(pos, std::memory_order_relaxed);
    }

    *eventCount = n;
    *dropCount = static_cast<int32_t>(eventBuffer.dropped.exchange(0, std::memory_order_relaxed));
    
    return UNECT_OK;
}

LIBUNECT_EXPORT UnectResult UNECT_CALL Unect_PollEvents(UnectSessionHandle session, UnectEvent* events, int32_t capacity, int32_t* eventCount, int32_t* dropCount)
{
    if (eventCount)
        *eventCount = 0;

    if (dropCount) 
        *dropCount = 0;

    if (!Unect_SessionValid(session))
        return UNECT_E_STALE_SESSION;

    return PopEvents(events, capacity, eventCount, dropCount);
}

#pragma endregion