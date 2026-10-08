/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/full_quantity/test_watch.c
 * PURPOSE: Exercise local watch transitions, expiration, cancellation and immutable policy ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiFullQuantityPolicy policy = Policy();
    UmiLiquidityObservation observation = Observation();
    UmiFullQuantityWatch *watch = NULL;
    UmiFullQuantityWatchSnapshot snapshot;
    if (!strcmp(argv[1], "bounds"))
    {
        CHECK(UmiFullQuantityWatchCreate(&policy, 9000U, 0U, &watch) == UMI_STATUS_INVALID_ARGUMENT &&
              watch == NULL);
        CHECK(UmiFullQuantityWatchCreate(&policy, UINT64_MAX, 1U, &watch) == UMI_STATUS_CAPACITY_EXCEEDED &&
              watch == NULL);
        CHECK(UmiFullQuantityWatchCreate(&policy, 9000U, 86400001U, &watch) == UMI_STATUS_INVALID_ARGUMENT &&
              watch == NULL);
        return 0;
    }
    CHECK(UmiFullQuantityWatchCreate(&policy, 9000U, 3000U, &watch) == UMI_STATUS_OK);
    if (!strcmp(argv[1], "transitions"))
    {
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9000U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_FILL_WATCH_MATCHED && snapshot.newlyMatched);
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9100U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_FILL_WATCH_MATCHED && !snapshot.newlyMatched);
        observation.visibleQuantity = Amount(10, 0);
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9200U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_FILL_WATCH_WAITING && !snapshot.newlyMatched);
        observation.visibleQuantity = Amount(7000, 0);
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9300U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.newlyMatched);
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 11000U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_FILL_WATCH_WAITING && !snapshot.review.displayedRuleMet);
    }
    else if (!strcmp(argv[1], "expiry"))
    {
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 12000U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_FILL_WATCH_EXPIRED && !snapshot.review.displayedRuleMet);
        observation.priceReceivedAtMilliseconds = 12001U;
        observation.sizeReceivedAtMilliseconds = 12001U;
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 12001U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_FILL_WATCH_EXPIRED && !snapshot.newlyMatched);
    }
    else if (!strcmp(argv[1], "cancel"))
    {
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9000U, &snapshot) == UMI_STATUS_OK);
        CHECK(UmiFullQuantityWatchCancel(watch) == UMI_STATUS_OK);
        CHECK(UmiFullQuantityWatchCancel(watch) == UMI_STATUS_OK);
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9100U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_FILL_WATCH_CANCELLED && !snapshot.newlyMatched &&
              !snapshot.review.displayedRuleMet);
    }
    else if (!strcmp(argv[1], "ownership"))
    {
        policy.quantity = Amount(1, 0);
        observation.visibleQuantity = Amount(10, 0);
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9000U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_FILL_WATCH_WAITING);
        CHECK(UmiFullQuantityWatchObserve(watch, NULL, 9100U, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.review.assessment == UMI_LIQUIDITY_UNAVAILABLE);
    }
    else if (!strcmp(argv[1], "rollback"))
    {
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9500U, &snapshot) == UMI_STATUS_OK);
        uint64_t revision = snapshot.revision;
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9400U, &snapshot) == UMI_STATUS_INVALID_STATE);
        observation.visibleQuantity.scale = 10U;
        CHECK(UmiFullQuantityWatchObserve(watch, &observation, 9600U, &snapshot) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFullQuantityWatchCopy(watch, &snapshot) == UMI_STATUS_OK && snapshot.revision == revision);
    }
    else
    {
        UmiFullQuantityWatchDestroy(watch);
        return 2;
    }
    UmiFullQuantityWatchDestroy(watch);
    return 0;
}
