#pragma once

// Preserve the protected implementation and expose a compatibility wrapper
// that corrects SensorValue without changing the remaining diagnostic fields.
#define Get_AF_Segment_Data Get_AF_Segment_Data_Impl
