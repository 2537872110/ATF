#include "AF_Internal.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <mutex>

namespace
{
    std::atomic<SensorValueCallback> g_clientSensorCallback(nullptr);
    std::mutex g_sensorFilterMutex;
    std::array<int, 9> g_sensorWindow = {{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }};
    int g_sensorWindowCount = 0;
    int g_sensorWindowIndex = 0;
    double g_sensorEma = 0.0;
    bool g_sensorEmaReady = false;
    int g_lastSensorOutput = 0;
    int g_pendingDirection = 0;
    int g_pendingFrames = 0;

    void ResetSensorFilter()
    {
        std::lock_guard<std::mutex> lock(g_sensorFilterMutex);
        g_sensorWindow.fill(0);
        g_sensorWindowCount = 0;
        g_sensorWindowIndex = 0;
        g_sensorEma = 0.0;
        g_sensorEmaReady = false;
        g_lastSensorOutput = 0;
        g_pendingDirection = 0;
        g_pendingFrames = 0;
    }

    void FilteredSensorCallback(int value)
    {
        int filteredValue = value;
        {
            std::lock_guard<std::mutex> lock(g_sensorFilterMutex);

            // Autofocus publishes zero as a state transition. Apply it
            // immediately and discard history so an old value cannot reappear.
            if (value == 0)
            {
                g_sensorWindow.fill(0);
                g_sensorWindowCount = 0;
                g_sensorWindowIndex = 0;
                g_sensorEma = 0.0;
                g_sensorEmaReady = true;
                g_lastSensorOutput = 0;
                g_pendingDirection = 0;
                g_pendingFrames = 0;
            }
            else
            {
            g_sensorWindow[g_sensorWindowIndex] = value;
            g_sensorWindowIndex = (g_sensorWindowIndex + 1) %
                static_cast<int>(g_sensorWindow.size());
            if (g_sensorWindowCount < static_cast<int>(g_sensorWindow.size()))
                ++g_sensorWindowCount;

            std::array<int, 9> sorted = g_sensorWindow;
            std::sort(sorted.begin(), sorted.begin() + g_sensorWindowCount);
            const int median = sorted[g_sensorWindowCount / 2];

            // During autofocus, show the robust median directly. The old slew
            // limiter made a transient error look like a slow mechanical
            // approach and kept decaying long after the axis had stopped.
            if (g_focusRunning.load())
            {
                filteredValue = median;
                g_sensorEma = static_cast<double>(median);
                g_sensorEmaReady = true;
                g_lastSensorOutput = median;
                g_pendingDirection = 0;
                g_pendingFrames = 0;
            }
            else
            {
            // A 9-frame median removes laser speckle and isolated centroid
            // errors. Use slower smoothing near the current output and faster
            // smoothing for a real displacement so Z movement remains visible.
            if (!g_sensorEmaReady)
            {
                g_sensorEma = static_cast<double>(median);
                g_sensorEmaReady = true;
                g_lastSensorOutput = median;
            }
            else
            {
                const int distance = std::abs(median - g_lastSensorOutput);
                const double alpha = distance >= 40 ? 0.55 :
                    (distance >= 16 ? 0.30 : 0.12);
                g_sensorEma += alpha *
                    (static_cast<double>(median) - g_sensorEma);
            }

            filteredValue = static_cast<int>(std::round(g_sensorEma));

            // Hold the displayed sensor value inside the normal stationary
            // noise band. A change outside that band must keep the same
            // direction for several frames before it is released.
            const int delta = filteredValue - g_lastSensorOutput;
            const int direction = (delta > 0) - (delta < 0);
            if (std::abs(delta) <= 6)
            {
                filteredValue = g_lastSensorOutput;
                g_pendingDirection = 0;
                g_pendingFrames = 0;
            }
            else
            {
                if (direction == g_pendingDirection)
                    ++g_pendingFrames;
                else
                {
                    g_pendingDirection = direction;
                    g_pendingFrames = 1;
                }

                const bool largeChange = std::abs(delta) >= 30;
                if (largeChange || g_pendingFrames >= 3)
                {
                    // Limit only display jumps; the autofocus controller still
                    // consumes the original unfiltered centroid measurement.
                    const int maxStep = largeChange ? 24 : 10;
                    const int step = std::max(-maxStep,
                        std::min(maxStep, delta));
                    g_lastSensorOutput += step;
                    filteredValue = g_lastSensorOutput;
                    g_pendingFrames = 0;
                }
                else
                {
                    filteredValue = g_lastSensorOutput;
                }
            }
            }
            }
        }

        SensorValueCallback callback = g_clientSensorCallback.load();
        if (callback != nullptr)
            callback(filteredValue);
    }
}

extern "C" __declspec(dllexport)
void RegisterSensorValueCallback(SensorValueCallback callback)
{
    g_clientSensorCallback.store(callback);
    ResetSensorFilter();
    g_sensorCb = callback != nullptr ? FilteredSensorCallback : nullptr;
}
