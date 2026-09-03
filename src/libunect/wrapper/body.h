#pragma once

#include "vector.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Kinect.h>

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

public:
    /// <summary>
    /// Initializes the instance from a kinect body pointer.
    /// </summary>
    /// <param name="body">The body to initialize the instance from.</param>
    void ConvertFrom(IBody* body) {
        if (!body)
            return;

        BOOLEAN tracked{ FALSE };
        body->get_IsTracked(&tracked);

        if (isTracked = static_cast<bool>(tracked); !isTracked)
            return;

        body->get_TrackingId(&trackingId);

        DWORD clipped{};
        HandState hs{ HandState_Unknown };
        TrackingConfidence tc{ TrackingConfidence_Low };

        body->get_ClippedEdges(&clipped);
        clippedEdges = static_cast<int32_t>(clipped);
        body->get_HandLeftState(&hs);        
        handLeftState = static_cast<int32_t>(hs);
        body->get_HandLeftConfidence(&tc);   
        handLeftConfidence = static_cast<int32_t>(tc);
        body->get_HandRightState(&hs);       
        handRightState = static_cast<int32_t>(hs);
        body->get_HandRightConfidence(&tc);  
        handRightConfidence = static_cast<int32_t>(tc);

        TrackingState leanTs{ TrackingState_NotTracked };
        body->get_LeanTrackingState(&leanTs);
        leanTrackingState = static_cast<int32_t>(leanTs);

        PointF lean{};
        body->get_Lean(&lean);
        lean = { lean.X, lean.Y };

        BOOLEAN restricted = FALSE;
        body->get_IsRestricted(&restricted);
        isRestricted = static_cast<bool>(restricted);

        ::Joint kinectJoints[JointType_Count]{};
        ::JointOrientation kinectJointOrientations[JointType_Count]{};

        body->GetJoints(JointType_Count, kinectJoints);
        body->GetJointOrientations(JointType_Count, kinectJointOrientations);

        for (uint32_t j{}; auto& joint : joints) {
            joint.position = { kinectJoints[j].Position.X, kinectJoints[j].Position.Y, kinectJoints[j].Position.Z };
            joint.orientation = { kinectJointOrientations[j].Orientation.x, kinectJointOrientations[j].Orientation.y, kinectJointOrientations[j].Orientation.z, kinectJointOrientations[j].Orientation.w };
            joint.trackingState = static_cast<int32_t>(kinectJoints[j].TrackingState);
            j++;
        }
    }

};