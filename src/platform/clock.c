#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif

#include "umicom/platform/clock.h"

#include <stdlib.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#include <errno.h>
#endif

typedef struct UmiFakeClockState {
    uint64_t now_nanoseconds;
} UmiFakeClockState;

static uint64_t system_monotonic(const UmiClock *clock)
{
    (void)clock;
#ifdef _WIN32
    LARGE_INTEGER counter;
    LARGE_INTEGER frequency;
    uint64_t counter_value;
    uint64_t frequency_value;
    uint64_t whole_seconds;
    uint64_t remainder;

    if (!QueryPerformanceCounter(&counter) ||
        !QueryPerformanceFrequency(&frequency) ||
        counter.QuadPart < 0 ||
        frequency.QuadPart <= 0) {
        return 0U;
    }

    counter_value = (uint64_t)counter.QuadPart;
    frequency_value = (uint64_t)frequency.QuadPart;
    whole_seconds = counter_value / frequency_value;
    remainder = counter_value % frequency_value;

    return whole_seconds * 1000000000ULL +
           (remainder * 1000000000ULL) / frequency_value;
#else
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) return 0U;
    return (uint64_t)value.tv_sec * 1000000000ULL + (uint64_t)value.tv_nsec;
#endif
}

static uint64_t system_wall(const UmiClock *clock)
{
    (void)clock;
#ifdef _WIN32
    FILETIME value;
    ULARGE_INTEGER ticks;
    GetSystemTimeAsFileTime(&value);
    ticks.LowPart = value.dwLowDateTime;
    ticks.HighPart = value.dwHighDateTime;
    return ticks.QuadPart * 100ULL;
#else
    struct timespec value;
    if (clock_gettime(CLOCK_REALTIME, &value) != 0) return 0U;
    return (uint64_t)value.tv_sec * 1000000000ULL + (uint64_t)value.tv_nsec;
#endif
}

static UmiStatus system_sleep(UmiClock *clock, uint64_t milliseconds)
{
    (void)clock;
#ifdef _WIN32
    Sleep((DWORD)milliseconds);
    return UMI_STATUS_OK;
#else
    struct timespec requested;
    requested.tv_sec = (time_t)(milliseconds / 1000ULL);
    requested.tv_nsec = (long)((milliseconds % 1000ULL) * 1000000ULL);
    while (nanosleep(&requested, &requested) != 0) {
        if (errno != EINTR) return UMI_STATUS_IO_ERROR;
    }
    return UMI_STATUS_OK;
#endif
}

static uint64_t fake_now(const UmiClock *clock)
{
    const UmiFakeClockState *state;
    if (clock == 0 || clock->state == 0) return 0U;
    state = (const UmiFakeClockState *)clock->state;
    return state->now_nanoseconds;
}

static UmiStatus fake_sleep(UmiClock *clock, uint64_t milliseconds)
{
    return umi_clock_fake_advance(clock, milliseconds * 1000000ULL);
}

UmiClock umi_clock_system(void)
{
    UmiClock clock;
    clock.monotonic_nanoseconds = system_monotonic;
    clock.wall_nanoseconds = system_wall;
    clock.sleep_milliseconds = system_sleep;
    clock.state = 0;
    return clock;
}

UmiClock umi_clock_fake(uint64_t initial_nanoseconds)
{
    UmiClock clock;
    UmiFakeClockState *state = (UmiFakeClockState *)calloc(1U, sizeof(*state));
    if (state != 0) state->now_nanoseconds = initial_nanoseconds;
    clock.monotonic_nanoseconds = fake_now;
    clock.wall_nanoseconds = fake_now;
    clock.sleep_milliseconds = fake_sleep;
    clock.state = state;
    return clock;
}

UmiStatus umi_clock_fake_set(UmiClock *clock, uint64_t nanoseconds)
{
    UmiFakeClockState *state;
    if (clock == 0 || clock->state == 0 || clock->monotonic_nanoseconds != fake_now)
        return UMI_STATUS_INVALID_ARGUMENT;
    state = (UmiFakeClockState *)clock->state;
    state->now_nanoseconds = nanoseconds;
    return UMI_STATUS_OK;
}

UmiStatus umi_clock_fake_advance(UmiClock *clock, uint64_t nanoseconds)
{
    UmiFakeClockState *state;
    if (clock == 0 || clock->state == 0 || clock->monotonic_nanoseconds != fake_now)
        return UMI_STATUS_INVALID_ARGUMENT;
    state = (UmiFakeClockState *)clock->state;
    state->now_nanoseconds += nanoseconds;
    return UMI_STATUS_OK;
}

void umi_clock_dispose(UmiClock *clock)
{
    if (clock == 0) return;
    if (clock->monotonic_nanoseconds == fake_now) free(clock->state);
    clock->state = 0;
    clock->monotonic_nanoseconds = 0;
    clock->wall_nanoseconds = 0;
    clock->sleep_milliseconds = 0;
}
