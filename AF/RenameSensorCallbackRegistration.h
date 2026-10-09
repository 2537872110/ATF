#pragma once

// Compile the protected registration function under an internal name. The
// public export is provided by AF_SensorFilter.cpp.
#define RegisterSensorValueCallback RegisterSensorValueCallback_Impl
