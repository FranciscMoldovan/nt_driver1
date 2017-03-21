#include "driver1.h"

#include <wdm.h>

#pragma warning(disable:4057)   // indirection to slightly different base type


__int32
DrvGetShortHexTimestamp(
    void )
{
    LARGE_INTEGER sysTime, localTime;
    TIME_FIELDS time;
    __int32 stamp;
    static BOOLEAN hexTableInited = FALSE;
    static __int32 hexTable[100];

    // pre-initialize hex table (on first use only)
    if (!hexTableInited)
    {
        __int32 i;

        for (i = 0; i < 100; i++)
        {
            hexTable[i] = ((i / 10) << 4) | (i % 10);
        }

        hexTableInited = TRUE;
    }

    KeQuerySystemTime(&sysTime);
    ExSystemTimeToLocalTime(&sysTime, &localTime);
    RtlTimeToTimeFields(&localTime, &time);

    // convert time to timestamp (hex-table optimized version)
    stamp =
        (hexTable[time.Hour] << 24) |
        (hexTable[time.Minute] << 16) |
        (hexTable[time.Second] << 8) |
        (hexTable[time.Milliseconds / 10]);

    return stamp;
}

#pragma warning(default:4057)
