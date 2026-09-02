#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Kinect.h>

#include <libunect.h>

static UnectResult StartStreaming(IKinectSensor* sensor, const UnectSessionDesc& sessionDesc) {
    return UNECT_E_NOT_IMPLEMENTED;
}

static UnectResult WidenStreams(IKinectSensor* sensor, UnectStreamType streams) {
    return UNECT_E_NOT_IMPLEMENTED;
}

static void StopStreams() {
    throw;
}

static void ForceUnlockStreams() {
    throw;
}