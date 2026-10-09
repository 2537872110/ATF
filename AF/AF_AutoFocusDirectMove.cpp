#include "AF_Internal.h"

extern "C" Result MoveZAutoFocusDirect(int requestedCorrection)
{
    // Preserve the explicit stop command used by AF_Stop().
    if (requestedCorrection == 0)
        return Move_Z(0);

    // g_lastErrorCount is updated from the newest centroid immediately before
    // the movement request. Cancel the full error instead of repeatedly
    // cancelling only PValue (normally 80 percent) of the remainder.
    const int fullCorrection = -g_lastErrorCount.load();
    const int correction =
        fullCorrection != 0 ? fullCorrection : requestedCorrection;
    const Result result = Move_Z(correction);
    if (result.code != 0 || g_zOutputMode.load() == 1)
        return result;

    // SET_PUL acknowledges command reception before the physical axis has
    // stopped. Do not let autofocus measure and enqueue another correction
    // while this move is still in progress.
    g_motionBusy.store(true);
    int settledSamples = 0;
    int failedPolls = 0;
    for (int elapsedMs = 0; elapsedMs < 1500; elapsedMs += 15)
    {
        Sleep(15);
        const auto statusResult = msg.GetStatues();
        if (!std::get<0>(statusResult))
        {
            if (++failedPolls >= 4)
                break;
            continue;
        }

        failedPolls = 0;
        const SStatue& status = std::get<1>(statusResult);
        const bool stopped =
            status.Statues.ZBusyStatue != EZBusyStatue::FocusRunning &&
            std::abs(status.Z_Vel) < 0.001f;
        settledSamples = stopped ? settledSamples + 1 : 0;
        if (settledSamples >= 2)
            break;
    }
    g_motionBusy.store(false);
    return result;
}
