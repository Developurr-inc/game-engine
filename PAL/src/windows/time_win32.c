//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include <PAL/time.h>

#ifdef N_PLATFORM_WINDOWS

# include <windows.h>

static float64 platform_get_absolute_time();
static void platform_sleep(const uint64 milliseconds);

Time *platform_time_create() {
    Time *time = platform_allocate(sizeof(Time), false);

    time->get_absolute_time = platform_get_absolute_time;
    time->sleep = platform_sleep;

    return time;
}

void platform_time_destroy(Time *time) {
    platform_free(time, false);
}

static float64 platform_get_absolute_time() {
    LARGE_INTEGER performance_counter;
    LARGE_INTEGER performance_frequency;

    QueryPerformanceCounter(&performance_counter);
    QueryPerformanceFrequency(&performance_frequency);

    return (float64) performance_counter.QuadPart / (float64) performance_frequency.QuadPart;
}

static void platform_sleep(const uint64 milliseconds) {
    Sleep((DWORD) milliseconds);
}

#endif
