//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include <PAL/time.h>

#ifdef N_PLATFORM_APPLE

# include <mach/mach_time.h>

float64 platform_get_absolute_time(void) {
    mach_timebase_info_data_t clock_timebase;
    mach_timebase_info(&clock_timebase);

    uint64 mach_absolute = mach_absolute_time();
    float64 nanos = (float64) mach_absolute * clock_timebase.numer / clock_timebase.denom;

    return nanos / 1.0e9; // Convert to seconds
}

void platform_sleep(uint64 milliseconds) {
# if _POSIX_C_SOURCE >= 199309L
    struct timespec time;

    time.tv_sec = milliseconds / 1000;
    time.tv_nsec = (milliseconds % 1000) * 1000 * 1000;

    nanosleep(&time, 0);
# else
    // if (milliseconds >= 1000)
    // {
    //     sleep((uint32)(milliseconds / 1000));
    // }
    //
    // usleep(milliseconds % 1000 * 1000);
# endif
}

#endif
