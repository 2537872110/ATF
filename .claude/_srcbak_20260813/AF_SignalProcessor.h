#pragma once

#include <array>

#include "opencv2/core.hpp"

enum class AFSegmentMode
{
    Auto = 0,
    AllSeven = 1,
    CenterThree = 2
};

struct AFSignalConfig
{
    int thresholdFloor;
    int saturationGray;
    int minSignalPixels;
    double thresholdRatio;
    AFSegmentMode mode;

    AFSignalConfig();
};

struct AFSegmentMeasurement
{
    std::array<double, 7> centroidX;
    std::array<double, 7> signalLevel;
    std::array<int, 7> valid;
    double aggregateCx;
    double quality;
    int validCount;
    bool saturated;

    AFSegmentMeasurement();
};

// ATF6-style signal processing:
// the same laser-line image is split into seven horizontal windows. Each
// window produces an independent line position and signal-level measurement.
class AFSignalProcessor
{
public:
    static bool Measure(
        const cv::Mat& source,
        const AFSignalConfig& config,
        AFSegmentMeasurement& result);
};
