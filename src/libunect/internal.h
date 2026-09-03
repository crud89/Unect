#pragma once

#include <atomic>

namespace Internal {

    /// <summary>
    /// Stores the global state of the sensor.
    /// </summary>
    extern std::atomic<int32_t> g_sensorState;

    /// <summary>
    /// The current epoch of the library runtime.
    /// </summary>
    extern std::atomic<uint32_t> g_epoch;

    /// <summary>
    /// Stores the current mapping generation.
    /// </summary>
    extern std::atomic<uint32_t> g_mappingGeneration;

}