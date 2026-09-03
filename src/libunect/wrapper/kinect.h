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