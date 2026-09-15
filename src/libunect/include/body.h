#pragma once

#include "vector.h"

#include <cstdint>
#include <array>

/// <summary>
/// Stores the number of joints per body.
/// </summary>
constexpr uint32_t UNECT_JOINT_COUNT = 25u;

/// <summary>
/// Stores the maximum number of bodies.
/// </summary>
constexpr uint32_t UNECT_BODY_COUNT = 6u;

/// <summary>
/// Stores information about a single joint.
/// </summary>
struct UnectJoint final {

    /// <summary>
    /// Stores the position of the joint in camera space.
    /// </summary>
    UnectVector3 position{};

    /// <summary>
    /// Stores the orientation of the joint as an absolute quaternion.
    /// </summary>
    UnectVector4 orientation{};

    /// <summary>
    /// Stores the tracking state of the joint (tracked, inferred, untracked).
    /// </summary>
    int32_t trackingState{};

};

/// <summary>
/// Stores body tracking data.
/// </summary>
struct Body final {

    /// <summary>
    /// Stores the tracking body ID.
    /// </summary>
    uint64_t trackingId{};
    
    /// <summary>
    /// Stores the edge clipping mask.
    /// </summary>
    int32_t clippedEdges{};

    /// <summary>
    /// Stores the tracking state for the right hand (tracked, inferred, untracked).
    /// </summary>
    int32_t handLeftState{};

    /// <summary>
    /// Stores the confidence for the left hand position.
    /// </summary>
    int32_t handLeftConfidence{};

    /// <summary>
    /// Stores the tracking state for the right hand (tracked, inferred, untracked).
    /// </summary>
    int32_t handRightState{};

    /// <summary>
    /// Stores the confidence for the right hand position.
    /// </summary>
    int32_t handRightConfidence{};

    /// <summary>
    /// Stores the <see cref="lean" /> tracking state (tracked, inferred, untracked).
    /// </summary>
    int32_t leanTrackingState{};

    /// <summary>
    /// Stores the lean vector of the body.
    /// </summary>
    UnectVector2 lean{};

    /// <summary>
    /// Stores the tracking state of the body.
    /// </summary>
    bool isTracked{};

    /// <summary>
    /// Stores the restricted state of the body.
    /// </summary>
    bool isRestricted{};

    /// <summary>
    /// Stores the joints of the body.
    /// </summary>
    std::array<UnectJoint, UNECT_JOINT_COUNT> joints{};

};