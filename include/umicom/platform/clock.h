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

typedef struct UmiClock UmiClock;
typedef uint64_t (*UmiClockNowFn)(const UmiClock *clock);
typedef UmiStatus (*UmiClockSleepFn)(UmiClock *clock, uint64_t milliseconds);

struct UmiClock {
    UmiClockNowFn monotonic_nanoseconds;
    UmiClockNowFn wall_nanoseconds;
    UmiClockSleepFn sleep_milliseconds;
    void *state;
};

UmiClock umi_clock_system(void);
UmiClock umi_clock_fake(uint64_t initial_nanoseconds);
UmiStatus umi_clock_fake_set(UmiClock *clock, uint64_t nanoseconds);
UmiStatus umi_clock_fake_advance(UmiClock *clock, uint64_t nanoseconds);
void umi_clock_dispose(UmiClock *clock);

#ifdef __cplusplus
}
#endif

#endif
