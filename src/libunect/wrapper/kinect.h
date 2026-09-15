#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Kinect.h>

#include <libunect.h>

/// <summary>
/// Starts streaming.
/// </summary>
/// <param name="sensor">A pointer to the sensor to stream from.</param>
/// <param name="sessionDesc">A descriptor for the current session.</param>
/// <returns>The return code of the function.</returns>
UnectResult StartStreaming(IKinectSensor* sensor, const UnectSessionDesc& sessionDesc);

/// <summary>
/// Opens additional streams.
/// </summary>
/// <param name="sensor">A pointer to the sensor to stream from.</param>
/// <param name="streams">A mask that indicates the streams to open.</param>
/// <returns>The return code of the function.</returns>
UnectResult WidenStreams(IKinectSensor* sensor, UnectStreamType streams);

/// <summary>
/// Stops streaming.
/// </summary>
void StopStreams();

/// <summary>
/// Forcefully unlocks all streams.
/// </summary>
void ForceUnlockStreams();

/// <summary>
/// Initializes a body instance from a kinect body pointer.
/// </summary>
/// <param name="body">The body to initialize the instance from.</param>
/// <returns>An instance of a body.</returns>
static inline Body ConvertFrom(IBody* body) {
    if (!body)
        return {};

    Body b{};

    BOOLEAN tracked{ FALSE };
    body->get_IsTracked(&tracked);

    if (b.isTracked = static_cast<bool>(tracked); !b.isTracked)
        return {};

    body->get_TrackingId(&b.trackingId);

    DWORD clipped{};
    HandState hs{ HandState_Unknown };
    TrackingConfidence tc{ TrackingConfidence_Low };

    body->get_ClippedEdges(&clipped);
    b.clippedEdges = static_cast<int32_t>(clipped);
    body->get_HandLeftState(&hs);
    b.handLeftState = static_cast<int32_t>(hs);
    body->get_HandLeftConfidence(&tc);
    b.handLeftConfidence = static_cast<int32_t>(tc);
    body->get_HandRightState(&hs);
    b.handRightState = static_cast<int32_t>(hs);
    body->get_HandRightConfidence(&tc);
    b.handRightConfidence = static_cast<int32_t>(tc);

    TrackingState leanTs{ TrackingState_NotTracked };
    body->get_LeanTrackingState(&leanTs);
    b.leanTrackingState = static_cast<int32_t>(leanTs);

    PointF lean{};
    body->get_Lean(&lean);
    b.lean = { lean.X, lean.Y };

    BOOLEAN restricted = FALSE;
    body->get_IsRestricted(&restricted);
    b.isRestricted = static_cast<bool>(restricted);

    ::Joint kinectJoints[JointType_Count]{};
    ::JointOrientation kinectJointOrientations[JointType_Count]{};

    body->GetJoints(JointType_Count, kinectJoints);
    body->GetJointOrientations(JointType_Count, kinectJointOrientations);

    for (uint32_t j{}; auto& joint : b.joints) {
        joint.position = { kinectJoints[j].Position.X, kinectJoints[j].Position.Y, kinectJoints[j].Position.Z };
        joint.orientation = { kinectJointOrientations[j].Orientation.x, kinectJointOrientations[j].Orientation.y, kinectJointOrientations[j].Orientation.z, kinectJointOrientations[j].Orientation.w };
        joint.trackingState = static_cast<int32_t>(kinectJoints[j].TrackingState);
        j++;
    }

    return b;
}