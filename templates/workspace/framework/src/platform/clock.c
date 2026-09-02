/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/platform/clock.c
 *
 * PURPOSE:
 *   Implement the clock behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
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

/* Provide the system monotonic operation used by this module and its client applications. */
static uint64_t system_monotonic(const UmiClock *clock)
{
    (void)clock;
#ifdef _WIN32
    LARGE_INTEGER counter;
    LARGE_INTEGER frequency;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!QueryPerformanceCounter(&counter) || !QueryPerformanceFrequency(&frequency)) return 0U;
    return (uint64_t)((counter.QuadPart * 1000000000ULL) / frequency.QuadPart);
#else
    struct timespec value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) return 0U;
    return (uint64_t)value.tv_sec * 1000000000ULL + (uint64_t)value.tv_nsec;
#endif
}

/* Provide the system wall operation used by this module and its client applications. */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock_gettime(CLOCK_REALTIME, &value) != 0) return 0U;
    return (uint64_t)value.tv_sec * 1000000000ULL + (uint64_t)value.tv_nsec;
#endif
}

/* Provide the system sleep operation used by this module and its client applications. */
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
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (nanosleep(&requested, &requested) != 0) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (errno != EINTR) return UMI_STATUS_IO_ERROR;
    }
    return UMI_STATUS_OK;
#endif
}

/* Provide the fake now operation used by this module and its client applications. */
static uint64_t fake_now(const UmiClock *clock)
{
    const UmiFakeClockState *state;
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock == 0 || clock->state == 0) return 0U;
    state = (const UmiFakeClockState *)clock->state;
    return state->now_nanoseconds;
}

/* Provide the fake sleep operation used by this module and its client applications. */
static UmiStatus fake_sleep(UmiClock *clock, uint64_t milliseconds)
{
    return umi_clock_fake_advance(clock, milliseconds * 1000000ULL);
}

/* Provide the clock system operation used by this module and its client applications. */
UmiClock umi_clock_system(void)
{
    UmiClock clock;
    clock.monotonic_nanoseconds = system_monotonic;
    clock.wall_nanoseconds = system_wall;
    clock.sleep_milliseconds = system_sleep;
    clock.state = 0;
    return clock;
}

/* Provide the clock fake operation used by this module and its client applications. */
UmiClock umi_clock_fake(uint64_t initial_nanoseconds)
{
    UmiClock clock;
    UmiFakeClockState *state = (UmiFakeClockState *)calloc(1U, sizeof(*state));
    /* Apply this branch only when its contract condition is satisfied. */
    if (state != 0) state->now_nanoseconds = initial_nanoseconds;
    clock.monotonic_nanoseconds = fake_now;
    clock.wall_nanoseconds = fake_now;
    clock.sleep_milliseconds = fake_sleep;
    clock.state = state;
    return clock;
}

/*
 * Copy clock fake into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_clock_fake_set(UmiClock *clock, uint64_t nanoseconds)
{
    UmiFakeClockState *state;
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock == 0 || clock->state == 0 || clock->monotonic_nanoseconds != fake_now)
        return UMI_STATUS_INVALID_ARGUMENT;
    state = (UmiFakeClockState *)clock->state;
    state->now_nanoseconds = nanoseconds;
    return UMI_STATUS_OK;
}

/*
 * Provide the clock fake advance operation used by this module and its client
 * applications.
 */
UmiStatus umi_clock_fake_advance(UmiClock *clock, uint64_t nanoseconds)
{
    UmiFakeClockState *state;
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock == 0 || clock->state == 0 || clock->monotonic_nanoseconds != fake_now)
        return UMI_STATUS_INVALID_ARGUMENT;
    state = (UmiFakeClockState *)clock->state;
    state->now_nanoseconds += nanoseconds;
    return UMI_STATUS_OK;
}

/* Release or reset state held by clock so the same storage can be reused safely. */
void umi_clock_dispose(UmiClock *clock)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock == 0) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock->monotonic_nanoseconds == fake_now) free(clock->state);
    clock->state = 0;
    clock->monotonic_nanoseconds = 0;
    clock->wall_nanoseconds = 0;
    clock->sleep_milliseconds = 0;
}
