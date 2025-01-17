//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include "../platform_detection.h"

#ifdef PLATFORM_MACOS
#include <PAL/time.h>

#include <mach/mach_time.h>

#if _POSIX_C_SOURCE >= 199309L
    #include <time.h>
#else
    #include <unistd.h>
#endif

float64 pal_get_absolute_time(void) {
    static mach_timebase_info_data_t timebase_info = {0};
    if (timebase_info.denom == 0) {
        mach_timebase_info(&timebase_info);
    }

    const float64 nanos = (float64) mach_absolute_time() * timebase_info.numer / timebase_info.denom;

    return nanos / 1.0e9;
}

void pal_sleep(uint64 milliseconds) {
    #if _POSIX_C_SOURCE >= 199309L
        struct timespec time;
        time.tv_sec = (__darwin_time_t) milliseconds / 1000;
        time.tv_nsec = ((__darwin_time_t) milliseconds % 1000) * 1000 * 1000;

        nanosleep(&time, nullptr);
    #else
        usleep(milliseconds % 1000 * 1000);
    #endif
}

#endif
