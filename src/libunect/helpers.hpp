#pragma once

#include <libunect.h>

/// <summary>
/// Validates the provided <paramref name="format" />.
/// </summary>
/// <param name="format">The format to validate.</param>
/// <returns>`true` if the format is valid and `false` otherwise.</returns>
constexpr bool ValidColorFormat(UnectColorFormat format) {
    return format == UNECT_COLOR_BGRA32
        || format == UNECT_COLOR_RGBA32
        || format == UNECT_COLOR_YUY2;
}

/// <summary>
/// Returns a new handle in a defined generation.
/// </summary>
/// <param name="generation">The handle generation (or epoch).</param>
/// <param name="id">The handle index.</param>
/// <returns>The unique handle value.</returns>
constexpr UnectSessionHandle MakeHandle(uint32_t generation, uint32_t id) {
    return (static_cast<UnectSessionHandle>(generation) << 32) | id;
}

/// <summary>
/// Returns the generation (or epoch) of the provided <paramref name="session" /> handle.
/// </summary>
/// <param name="session">The session handle to check.</param>
/// <returns>The generation (or epoch) of the session handle.</returns>
constexpr uint32_t HandleGeneration(UnectSessionHandle session) {
    return static_cast<uint32_t>(session >> 32);
}

/// <summary>
/// Returns the identifier of the provided <paramref name="session" /> handle.
/// </summary>
/// <param name="session">The session handle to acquire the identifier from.</param>
/// <returns>The identifier of the provided sessoin handle.</returns>
constexpr uint32_t HandleIdentifier(UnectSessionHandle session) {
    return static_cast<uint32_t>(session & 0xFFFFFFFFu);
}