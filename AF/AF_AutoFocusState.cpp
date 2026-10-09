#include "AF_Internal.h"

void PublishAutoFocusState(int value)
{
    PublishState(value);

    if (value == 0 && g_sensorCb != nullptr)
        g_sensorCb(0);
}
