//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include <PAL/time.h>

#ifdef N_PLATFORM_LINUX

# include <sys/time.h>

# if _POSIX_C_SOURCE >= 199309L
#  include <time.h>
# else
#  include <unistd.h>
# endif

float64 platform_get_absolute_time() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);

    return (float64)time.tv_sec + (float64)time.tv_nsec / 1000000000.0;
}

void platform_sleep(uint64 milliseconds) {
# if _POSIX_C_SOURCE >= 199309L
    struct timespec time;

    time.tv_sec = milliseconds / 1000;
    time.tv_nsec = (milliseconds % 1000) * 1000 * 1000;

    nanosleep(&time, 0);
# else
    if (milliseconds >= 1000)
    {
        sleep(milliseconds / 1000);
    }

    usleep(milliseconds % 1000 * 1000);
# endif
}

#endif
