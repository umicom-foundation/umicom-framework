/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/clock.h
 *
 * PURPOSE:
 *   Publish the public clock contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CLOCK_H
#define UMICOM_PLATFORM_CLOCK_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the clock data shared with callers of this public contract.
 */
typedef struct UmiClock UmiClock;
typedef uint64_t (*UmiClockNowFn)(const UmiClock *clock);
typedef UmiStatus (*UmiClockSleepFn)(UmiClock *clock, uint64_t milliseconds);

struct UmiClock {
    UmiClockNowFn monotonic_nanoseconds;
    UmiClockNowFn wall_nanoseconds;
    UmiClockSleepFn sleep_milliseconds;
    void *state;
};

/**
 * Provide the clock system operation used by this module and its client applications.
 */
UmiClock umi_clock_system(void);
/**
 * Provide the clock fake operation used by this module and its client applications.
 */
UmiClock umi_clock_fake(uint64_t initial_nanoseconds);
/**
 * Copy clock fake into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_clock_fake_set(UmiClock *clock, uint64_t nanoseconds);
/**
 * Provide the clock fake advance operation used by this module and its client
 * applications.
 */
UmiStatus umi_clock_fake_advance(UmiClock *clock, uint64_t nanoseconds);
/**
 * Release or reset state held by clock so the same storage can be reused safely.
 */
void umi_clock_dispose(UmiClock *clock);

#ifdef __cplusplus
}
#endif

#endif
