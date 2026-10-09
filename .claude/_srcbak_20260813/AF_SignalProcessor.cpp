#include "AF_SignalProcessor.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

#include "opencv2/imgproc.hpp"

namespace
{
    double Median(std::vector<double> values)
    {
        if (values.empty())
            return 0.0;

        const size_t middle = values.size() / 2;
        std::nth_element(values.begin(), values.begin() + middle, values.end());
        double median = values[middle];
        if ((values.size() % 2) == 0)
        {
            std::nth_element(values.begin(), values.begin() + middle - 1, values.end());
            median = (median + values[middle - 1]) * 0.5;
        }
        return median;
    }

    bool FitLine(
        const std::vector<int>& indices,
        const std::array<double, 7>& values,
        double& slope,
        double& intercept)
    {
        if (indices.empty())
            return false;

        if (indices.size() == 1)
        {
            slope = 0.0;
            intercept = values[indices[0]];
            return true;
        }

        double meanX = 0.0;
        double meanY = 0.0;
        for (size_t i = 0; i < indices.size(); ++i)
        {
            meanX += indices[i];
            meanY += values[indices[i]];
        }
        meanX /= static_cast<double>(indices.size());
        meanY /= static_cast<double>(indices.size());

        double numerator = 0.0;
        double denominator = 0.0;
        for (size_t i = 0; i < indices.size(); ++i)
        {
            const double dx = indices[i] - meanX;
            numerator += dx * (values[indices[i]] - meanY);
            denominator += dx * dx;
        }

        if (denominator <= 0.0)
            return false;

        slope = numerator / denominator;
        intercept = meanY - slope * meanX;
        return true;
    }

    double Percentile90(std::vector<unsigned char>& values)
    {
        if (values.empty())
            return 0.0;

        const size_t index =
            static_cast<size_t>(0.90 * static_cast<double>(values.size() - 1));
        std::nth_element(values.begin(), values.begin() + index, values.end());
        return values[index];
    }
}

AFSignalConfig::AFSignalConfig()
    : thresholdFloor(20),
      saturationGray(245),
      minSignalPixels(8),
      thresholdRatio(0.25),
      mode(AFSegmentMode::Auto)
{
}

AFSegmentMeasurement::AFSegmentMeasurement()
    : aggregateCx(-1.0),
      quality(0.0),
      validCount(0),
      saturated(false)
{
    centroidX.fill(-1.0);
    signalLevel.fill(0.0);
    valid.fill(0);
}

bool AFSignalProcessor::Measure(
    const cv::Mat& source,
    const AFSignalConfig& config,
    AFSegmentMeasurement& result)
{
    result = AFSegmentMeasurement();
    if (source.empty() || source.cols < 8 || source.rows < 14)
        return false;

    cv::Mat gray;
    if (source.channels() == 1)
        gray = source;
    else if (source.channels() == 3)
        cv::cvtColor(source, gray, cv::COLOR_BGR2GRAY);
    else if (source.channels() == 4)
        cv::cvtColor(source, gray, cv::COLOR_BGRA2GRAY);
    else
        return false;

    if (gray.depth() != CV_8U)
        gray.convertTo(gray, CV_8U);

    const double ratio =
        std::max(0.05, std::min(0.95, config.thresholdRatio));

    for (int segment = 0; segment < 7; ++segment)
    {
        const int rowBegin = gray.rows * segment / 7;
        const int rowEnd = gray.rows * (segment + 1) / 7;
        if (rowEnd <= rowBegin)
            continue;

        double maximum = 0.0;
        cv::minMaxLoc(gray.rowRange(rowBegin, rowEnd), NULL, &maximum);
        const double threshold =
            std::max(static_cast<double>(config.thresholdFloor), maximum * ratio);

        double weightedX = 0.0;
        double weightSum = 0.0;
        int signalPixels = 0;
        int saturatedPixels = 0;
        std::vector<unsigned char> brightValues;

        for (int row = rowBegin; row < rowEnd; ++row)
        {
            const unsigned char* pixels = gray.ptr<unsigned char>(row);
            for (int column = 0; column < gray.cols; ++column)
            {
                const int value = pixels[column];
                if (value < threshold)
                    continue;

                const double weight = value - threshold + 1.0;
                weightedX += column * weight;
                weightSum += weight;
                ++signalPixels;
                brightValues.push_back(pixels[column]);
                if (value >= config.saturationGray)
                    ++saturatedPixels;
            }
        }

        result.signalLevel[segment] = Percentile90(brightValues);
        if (signalPixels < config.minSignalPixels || weightSum <= 0.0)
            continue;

        result.centroidX[segment] = weightedX / weightSum;
        result.valid[segment] = 1;
        if (saturatedPixels * 5 > signalPixels)
            result.saturated = true;
    }

    std::vector<int> selected;
    if (config.mode == AFSegmentMode::CenterThree)
    {
        for (int i = 2; i <= 4; ++i)
            if (result.valid[i])
                selected.push_back(i);
    }
    else
    {
        for (int i = 0; i < 7; ++i)
            if (result.valid[i])
                selected.push_back(i);
    }

    const int required =
        config.mode == AFSegmentMode::CenterThree ? 2 : 3;
    if (static_cast<int>(selected.size()) < required)
        return false;

    // A tilted laser line naturally has a different X position in each
    // horizontal window. Fit a line across the seven positions and reject
    // only points that depart from that line, rather than rejecting the tilt.
    double lineSlope = 0.0;
    double lineIntercept = 0.0;
    if (!FitLine(selected, result.centroidX, lineSlope, lineIntercept))
        return false;

    if (selected.size() >= 4)
    {
        std::vector<double> residuals;
        residuals.reserve(selected.size());
        for (size_t i = 0; i < selected.size(); ++i)
        {
            const int index = selected[i];
            const double predicted = lineSlope * index + lineIntercept;
            residuals.push_back(std::abs(result.centroidX[index] - predicted));
        }

        const double tolerance = std::max(2.0, Median(residuals) * 3.5);
        std::vector<int> filtered;
        for (size_t i = 0; i < selected.size(); ++i)
        {
            if (residuals[i] <= tolerance)
                filtered.push_back(selected[i]);
            else
                result.valid[selected[i]] = 0;
        }

        if (static_cast<int>(filtered.size()) >= required)
        {
            selected.swap(filtered);
            FitLine(selected, result.centroidX, lineSlope, lineIntercept);
        }
    }

    double residualSquareSum = 0.0;
    double signalSum = 0.0;
    for (size_t i = 0; i < selected.size(); ++i)
    {
        const int index = selected[i];
        const double residual =
            result.centroidX[index] - (lineSlope * index + lineIntercept);
        residualSquareSum += residual * residual;
        signalSum += result.signalLevel[index];
    }

    result.validCount = static_cast<int>(selected.size());
    result.aggregateCx = lineSlope * 3.0 + lineIntercept;

    const double rmsResidual =
        std::sqrt(residualSquareSum / static_cast<double>(selected.size()));
    const double countQuality =
        std::min(1.0, selected.size() /
            static_cast<double>(config.mode == AFSegmentMode::CenterThree ? 3 : 7));
    const double consistencyQuality = 1.0 / (1.0 + rmsResidual / 3.0);
    const double meanSignal = signalSum / static_cast<double>(selected.size());
    const double signalQuality =
        std::max(0.0, std::min(1.0,
            (meanSignal - config.thresholdFloor) /
            std::max(1.0, 120.0 - config.thresholdFloor)));
    const double saturationPenalty = result.saturated ? 0.75 : 1.0;
    result.quality =
        countQuality * consistencyQuality * signalQuality * saturationPenalty;

    return result.aggregateCx >= 0.0 &&
        result.aggregateCx < static_cast<double>(gray.cols);
}
