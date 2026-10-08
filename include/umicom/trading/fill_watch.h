/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/fill_watch.h
 * PURPOSE: Own a bounded local liquidity watch without order execution authority.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_FILL_WATCH_H
#define UMICOM_TRADING_FILL_WATCH_H
#include "umicom/trading/fill_policy.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiFullQuantityWatchState
    {
        UMI_FILL_WATCH_WAITING = 0,
        UMI_FILL_WATCH_MATCHED,
        UMI_FILL_WATCH_EXPIRED,
        UMI_FILL_WATCH_CANCELLED
    } UmiFullQuantityWatchState;
    typedef struct UmiFullQuantityWatchSnapshot
    {
        UmiFullQuantityWatchState state;
        UmiFullQuantityReview review;
        uint64_t expiresAtMilliseconds, observedAtMilliseconds, revision;
        bool newlyMatched;
    } UmiFullQuantityWatchSnapshot;
    typedef struct UmiFullQuantityWatch UmiFullQuantityWatch;

    /* Copy the policy into an owned, single-threaded watch. Duration is 1..86400000
 * milliseconds. No timers, sockets, orders or files are created. The host must
 * observe regularly, including when data stops arriving, and destroy/replace
 * this object on a connection, contract or subscription change. Failure sets
 * *out to NULL. A matched watch is a notification, never an instruction to trade. */
    UmiStatus UmiFullQuantityWatchCreate(const UmiFullQuantityPolicy *policy, uint64_t nowMilliseconds,
                                         uint64_t durationMilliseconds, UmiFullQuantityWatch **out);
    void UmiFullQuantityWatchDestroy(UmiFullQuantityWatch *watch);
    /* Null evidence means unavailable. Expired/cancelled watches stay terminal.
 * Invalid evidence, reversed time or revision exhaustion preserves state/out.
 * newlyMatched is true only for a transition from waiting to matched. Losing
 * freshness, size or price eligibility returns a matched watch to waiting. */
    UmiStatus UmiFullQuantityWatchObserve(UmiFullQuantityWatch *watch,
                                          const UmiLiquidityObservation *observation,
                                          uint64_t nowMilliseconds, UmiFullQuantityWatchSnapshot *out);
    UmiStatus UmiFullQuantityWatchCancel(UmiFullQuantityWatch *watch);
    UmiStatus UmiFullQuantityWatchCopy(const UmiFullQuantityWatch *watch, UmiFullQuantityWatchSnapshot *out);
#ifdef __cplusplus
}
#endif
#endif
