/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/task_wait.inc
 *
 * PURPOSE:
 *   Share total wait-budget accounting between task and queue owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/clock.h"

/* The existing native condition API performs the sleep. This helper prevents
 * spurious wakes from restarting a finite timeout. It is not a real-time
 * scheduling guarantee; native scheduling and mutex acquisition can run late. */
static uint32_t UmiTaskWaitRemaining(const UmiClock *clock,
    uint64_t startTime, uint32_t timeoutMs)
{
    uint64_t now, elapsed, budget;
    if (timeoutMs == 0U) return 0U;
    now = clock->monotonic_nanoseconds(clock);
    budget = (uint64_t)timeoutMs * 1000000ULL;
    if (now < startTime) return 0U;
    elapsed = now - startTime;
    if (elapsed >= budget) return 0U;
    return (uint32_t)((budget - elapsed + 999999ULL) / 1000000ULL);
}
