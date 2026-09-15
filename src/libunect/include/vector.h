#pragma once

/// <summary>
/// Represents a 2D floating-point vector.
/// </summary>
typedef struct UnectVector2 final {

    /// <summary>
    /// Stores the x-coordinate of the vector.
    /// </summary>
    float x{};

    /// <summary>
    /// Stores the y-coordinate of the vector.
    /// </summary>
    float y{};

} UnectVector2;

/// <summary>
/// Represents a 3D floating-point vector.
/// </summary>
typedef struct UnectVector3 final {

    /// <summary>
    /// Stores the x-coordinate of the vector.
    /// </summary>
    float x{};

    /// <summary>
    /// Stores the y-coordinate of the vector.
    /// </summary>
    float y{};

    /// <summary>
    /// Stores the z-coordinate of the vector.
    /// </summary>
    float z{};

} UnectVector3;

/// <summary>
/// Represents a 4D floating-point vector.
/// </summary>
typedef struct UnectVector4 final {

    /// <summary>
    /// Stores the x-coordinate of the vector.
    /// </summary>
    float x{};

    /// <summary>
    /// Stores the y-coordinate of the vector.
    /// </summary>
    float y{};

    /// <summary>
    /// Stores the z-coordinate of the vector.
    /// </summary>
    float z{};

    /// <summary>
    /// Stores the w-coordinate of the vector.
    /// </summary>
    float w{};

} UnectVector4;