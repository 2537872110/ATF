#pragma once

// The sensor display is a centroid delta in DU. Keep the slope-based count
// conversion used by the autofocus controller separate from this display map.
#define MapCxToSensorValue MapCxToSensorDisplayValue
