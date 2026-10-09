#pragma once

#include <MvCameraControl.h>

// Compatibility for the protected legacy source: Reset_Roi stops image
// acquisition and must resume acquisition, not start video recording.
#define MV_CC_StartRecord(handle) MV_CC_StartGrabbing(handle)
