#pragma once

// Redirect Z moves only inside AF_AutoFocus.cpp. The wrapper replaces the
// legacy proportional terminal step with the full calibrated correction.
#define Move_Z MoveZAutoFocusDirect
