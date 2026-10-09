#include "AF_Main.h"

#include <mutex>

extern void* handle;
extern "C" Result Set_Focus_Mode_Impl(int mode);

extern "C" __declspec(dllexport) Result Set_Focus_Mode(int mode)
{
    static std::mutex cacheMutex;
    static void* appliedHandle = nullptr;
    static int appliedMode = -1;
    static char okMessage[] = "ok";

    std::lock_guard<std::mutex> lock(cacheMutex);

    // Reapplying the same mode used to stop acquisition, rewrite the ROI and
    // restart acquisition on every AF button click. Skip that expensive cycle
    // when neither the camera instance nor the requested mode has changed.
    if (handle != nullptr && handle == appliedHandle && mode == appliedMode)
    {
        Result cachedResult;
        cachedResult.code = 0;
        cachedResult.message = okMessage;
        return cachedResult;
    }

    Result result = Set_Focus_Mode_Impl(mode);
    if (result.code == 0)
    {
        appliedHandle = handle;
        appliedMode = mode;
    }
    return result;
}
