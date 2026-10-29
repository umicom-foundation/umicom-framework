/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/reply_deadline_private.h
 *
 * PURPOSE:
 *   Keep correlated reply waits inside one positive elapsed-time budget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LANGUAGE_REPLY_DEADLINE_PRIVATE_H
#define UMICOM_LANGUAGE_REPLY_DEADLINE_PRIVATE_H
#include "umicom/platform/clock.h"

/* Zero denotes an immediate poll and is handled by each caller's poll boundary.
 * A positive budget is checked before and after a receive: a synchronous
 * transport cannot turn a late complete frame into an on-time response. */
static inline int UmiLanguageReplyBudgetReached(const UmiClock *clock,
    uint64_t started, uint32_t timeout_ms)
{
    return timeout_ms != 0U &&
        (clock->monotonic_nanoseconds(clock) - started) / UINT64_C(1000000) >= timeout_ms;
}
#endif
