/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/deadline.h
 * PURPOSE: Share one monotonic timeout budget across a DAP operation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DEADLINE_H
#define UMICOM_DEBUG_RUNTIME_DEADLINE_H
#include "umicom/platform/clock.h"
typedef struct DebugDeadline { UmiClock clock; uint64_t start; uint32_t timeout; } DebugDeadline;
static inline DebugDeadline DebugDeadlineStart(uint32_t timeout) {
    DebugDeadline d; d.clock = umi_clock_system();
    d.start = d.clock.monotonic_nanoseconds(&d.clock); d.timeout = timeout; return d;
}
static inline uint32_t DebugDeadlineRemaining(DebugDeadline *d) {
    uint64_t elapsed = (d->clock.monotonic_nanoseconds(&d->clock) - d->start) / 1000000U;
    return elapsed >= d->timeout ? 0U : d->timeout - (uint32_t)elapsed;
}
#endif
