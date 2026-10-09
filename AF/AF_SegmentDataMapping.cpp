#include "AF_Internal.h"

#include <algorithm>
#include <cmath>

extern "C" Result Get_AF_Segment_Data_Impl(AFSegmentData* outData);

extern "C" __declspec(dllexport)
Result Get_AF_Segment_Data(AFSegmentData* outData)
{
    Result result = Get_AF_Segment_Data_Impl(outData);
    if (result.code != 0 || outData == nullptr)
        return result;

    LensParameter parameter;
    if (!GetCurrentLensSnapshot(parameter))
    {
        outData->SensorValue = 0;
        return result;
    }

    const double calibratedCx = parameter.HasCentroid_X
        ? parameter.Centroid_X
        : (parameter.HasOrigin_Centroid_X
            ? parameter.Origin_Centroid_X
            : outData->AggregateCentroidX);

    // Movement-graph Sensor Output is the centroid displacement itself.
    // Slope and LinearRangeCount belong only to Z error conversion.
    const double deltaCx = outData->AggregateCentroidX - calibratedCx;
    outData->SensorValue = static_cast<int>(std::max(
        -512.0,
        std::min(511.0, std::round(deltaCx))));
    return result;
}
