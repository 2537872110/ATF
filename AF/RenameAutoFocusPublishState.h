#pragma once

// Redirect state publication only inside AF_AutoFocus.cpp so a successful
// autofocus state can also zero the sensor display.
#define PublishState PublishAutoFocusState
