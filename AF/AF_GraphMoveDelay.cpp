#include "AF_Internal.h"

#include <atomic>
#include <cmath>

extern "C" Result Move_Z_Impl(int value);

namespace
{
    std::atomic<bool> g_graphScanDelayEnabled(false);
    std::atomic<int> g_graphScanStart(0);
    std::atomic<int> g_graphScanEnd(0);
    std::atomic<int> g_graphScanPosition(0);
}

extern "C" __declspec(dllexport)
void Configure_Graph_Scan_Delay(int start, int end)
{
    g_graphScanStart.store(start);
    g_graphScanEnd.store(end);
    g_graphScanPosition.store(0);
    g_graphScanDelayEnabled.store(true);
}

extern "C" __declspec(dllexport)
Result Move_Z(int value)
{
    Result result = Move_Z_Impl(value);
    if (result.code != 0 || !g_graphScanDelayEnabled.load())
        return result;

    const int position = g_graphScanPosition.fetch_add(value) + value;
    if (position == g_graphScanStart.load() ||
        position == g_graphScanEnd.load())
    {
        Sleep(5000);
    }

    // The graph worker returns by cancelling its complete accumulated offset.
    if (position == 0 && value != 0)
        g_graphScanDelayEnabled.store(false);
    return result;
}
