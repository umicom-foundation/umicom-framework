/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/fill_watch.c
 * PURPOSE: Keep local liquidity observations separate from broker submission and order lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/fill_watch.h"
#include <stdlib.h>

struct UmiFullQuantityWatch
{
    UmiFullQuantityPolicy policy;
    UmiFullQuantityWatchSnapshot snapshot;
};

UmiStatus UmiFullQuantityWatchCreate(const UmiFullQuantityPolicy *policy, uint64_t nowMilliseconds,
                                     uint64_t durationMilliseconds, UmiFullQuantityWatch **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = UmiFullQuantityPolicyValidate(policy);
    if (status != UMI_STATUS_OK)
        return status;
    if (durationMilliseconds == 0U || durationMilliseconds > UINT64_C(86400000))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (nowMilliseconds > UINT64_MAX - durationMilliseconds)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiFullQuantityWatch *watch = calloc(1U, sizeof *watch);
    if (watch == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    watch->policy = *policy;
    watch->snapshot.expiresAtMilliseconds = nowMilliseconds + durationMilliseconds;
    watch->snapshot.observedAtMilliseconds = nowMilliseconds;
    watch->snapshot.revision = 1U;
    *out = watch;
    return UMI_STATUS_OK;
}
void UmiFullQuantityWatchDestroy(UmiFullQuantityWatch *watch) { free(watch); }

UmiStatus UmiFullQuantityWatchObserve(UmiFullQuantityWatch *watch, const UmiLiquidityObservation *observation,
                                      uint64_t nowMilliseconds, UmiFullQuantityWatchSnapshot *out)
{
    if (watch == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (nowMilliseconds < watch->snapshot.observedAtMilliseconds)
        return UMI_STATUS_INVALID_STATE;
    if (watch->snapshot.state == UMI_FILL_WATCH_EXPIRED || watch->snapshot.state == UMI_FILL_WATCH_CANCELLED)
    {
        *out = watch->snapshot;
        return UMI_STATUS_OK;
    }
    if (watch->snapshot.revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiFullQuantityWatchSnapshot next = watch->snapshot;
    next.newlyMatched = false;
    if (nowMilliseconds >= next.expiresAtMilliseconds)
    {
        next.state = UMI_FILL_WATCH_EXPIRED;
        next.review = (UmiFullQuantityReview){0};
    }
    else
    {
        const UmiLiquidityObservation missing = {0};
        UmiStatus status = UmiFullQuantityEvaluate(
            &watch->policy, observation != NULL ? observation : &missing, nowMilliseconds, &next.review);
        if (status != UMI_STATUS_OK)
            return status;
        next.state = next.review.displayedRuleMet ? UMI_FILL_WATCH_MATCHED : UMI_FILL_WATCH_WAITING;
        next.newlyMatched =
            next.state == UMI_FILL_WATCH_MATCHED && watch->snapshot.state != UMI_FILL_WATCH_MATCHED;
    }
    next.observedAtMilliseconds = nowMilliseconds;
    ++next.revision;
    watch->snapshot = next;
    *out = next;
    return UMI_STATUS_OK;
}
UmiStatus UmiFullQuantityWatchCancel(UmiFullQuantityWatch *watch)
{
    if (watch == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (watch->snapshot.state == UMI_FILL_WATCH_CANCELLED || watch->snapshot.state == UMI_FILL_WATCH_EXPIRED)
        return UMI_STATUS_OK;
    if (watch->snapshot.revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    watch->snapshot.state = UMI_FILL_WATCH_CANCELLED;
    watch->snapshot.review = (UmiFullQuantityReview){0};
    watch->snapshot.newlyMatched = false;
    ++watch->snapshot.revision;
    return UMI_STATUS_OK;
}
UmiStatus UmiFullQuantityWatchCopy(const UmiFullQuantityWatch *watch, UmiFullQuantityWatchSnapshot *out)
{
    if (watch == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = watch->snapshot;
    return UMI_STATUS_OK;
}
