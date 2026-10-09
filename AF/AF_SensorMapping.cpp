#include "AF_Internal.h"

#include <algorithm>
#include <cmath>

int MapCxToSensorDisplayValue(
    double nowCX,
    double calibratedCx,
    const LensParameter&)
{
    if (!std::isfinite(nowCX) || !std::isfinite(calibratedCx) || nowCX < 0.0)
        return 0;

    // CX is an image coordinate, but its calibrated focus position does not
    // need to be at the image centre. One pixel of displacement equals one DU.
    const double deltaCx = nowCX - calibratedCx;
    return static_cast<int>(std::max(
        -512.0,
        std::min(511.0, std::round(deltaCx))));
}
