/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/full_quantity/test_policy.c
 * PURPOSE: Check economic intent, quote age and field-skew refusals without broker I/O.
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
    UmiFullQuantityReview review;
    memset(&review, 0x5a, sizeof review);
    UmiLiquidityAssessment expected = UMI_LIQUIDITY_MEETS_DISPLAYED_RULE;
    if (!strcmp(argv[1], "partial-size"))
    {
        observation.visibleQuantity = Amount(10, 0);
        expected = UMI_LIQUIDITY_INSUFFICIENT;
    }
    else if (!strcmp(argv[1], "fraction"))
    {
        observation.visibleQuantity = Amount(6999999, 3);
        expected = UMI_LIQUIDITY_INSUFFICIENT;
    }
    else if (!strcmp(argv[1], "buffer"))
    {
        policy.extraVisibleQuantity = Amount(1000, 0);
        expected = UMI_LIQUIDITY_INSUFFICIENT;
    }
    else if (!strcmp(argv[1], "price-buy"))
    {
        observation.price = Amount(42501, 4);
        expected = UMI_LIQUIDITY_OUTSIDE_LIMIT;
    }
    else if (!strcmp(argv[1], "price-sell"))
    {
        policy.side = UMI_SIDE_SELL;
        observation.price = Amount(42499, 4);
        expected = UMI_LIQUIDITY_OUTSIDE_LIMIT;
    }
    else if (!strcmp(argv[1], "sell-better"))
    {
        policy.side = UMI_SIDE_SELL;
        observation.price = Amount(426, 2);
    }
    else if (!strcmp(argv[1], "buy-better"))
        observation.price = Amount(424, 2);
    else if (!strcmp(argv[1], "missing"))
    {
        observation.available = false;
        expected = UMI_LIQUIDITY_UNAVAILABLE;
    }
    else if (!strcmp(argv[1], "delayed"))
    {
        observation.realtime = false;
        expected = UMI_LIQUIDITY_NOT_REALTIME;
    }
    else if (!strcmp(argv[1], "stale-flag"))
    {
        observation.stale = true;
        expected = UMI_LIQUIDITY_STALE;
    }
    else if (!strcmp(argv[1], "old-price"))
    {
        observation.priceReceivedAtMilliseconds = 8000U;
        expected = UMI_LIQUIDITY_STALE;
    }
    else if (!strcmp(argv[1], "old-size"))
    {
        observation.sizeReceivedAtMilliseconds = 8000U;
        expected = UMI_LIQUIDITY_STALE;
    }
    else if (!strcmp(argv[1], "future"))
    {
        observation.priceReceivedAtMilliseconds = 10001U;
        expected = UMI_LIQUIDITY_CLOCK_MISMATCH;
    }
    else if (!strcmp(argv[1], "skew"))
    {
        observation.sizeReceivedAtMilliseconds = 9501U;
        expected = UMI_LIQUIDITY_FIELDS_SKEWED;
    }
    else if (!strcmp(argv[1], "boundary-age"))
    {
        observation.priceReceivedAtMilliseconds = 8500U;
        observation.sizeReceivedAtMilliseconds = 8500U;
    }
    else if (!strcmp(argv[1], "boundary-skew"))
        observation.sizeReceivedAtMilliseconds = 9500U;
    else if (!strcmp(argv[1], "zero-size"))
    {
        observation.visibleQuantity = Amount(0, 0);
        expected = UMI_LIQUIDITY_INSUFFICIENT;
    }
    else if (!strcmp(argv[1], "invalid"))
    {
        unsigned char before[sizeof review];
        memcpy(before, &review, sizeof review);
        policy.side = (UmiSide)0;
        CHECK(UmiFullQuantityEvaluate(&policy, &observation, 10000U, &review) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&before, &review, sizeof review));
        policy = Policy();
        policy.quantity.coefficient = 0;
        CHECK(UmiFullQuantityPolicyValidate(&policy) == UMI_STATUS_INVALID_ARGUMENT);
        policy = Policy();
        policy.maximumSkewMilliseconds = 1501U;
        CHECK(UmiFullQuantityPolicyValidate(&policy) == UMI_STATUS_INVALID_ARGUMENT);
        policy = Policy();
        policy.extraVisibleQuantity.coefficient = -1;
        CHECK(UmiFullQuantityPolicyValidate(&policy) == UMI_STATUS_INVALID_ARGUMENT);
        policy = Policy();
        policy.waitingTimeInForce = UMI_TIF_IOC;
        CHECK(UmiFullQuantityPolicyValidate(&policy) == UMI_STATUS_INVALID_ARGUMENT);
        policy = Policy();
        policy.quantity = Amount(INT64_MAX, 0);
        policy.extraVisibleQuantity = Amount(1, 0);
        CHECK(UmiFullQuantityEvaluate(&policy, &observation, 10000U, &review) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!memcmp(&before, &review, sizeof review));
        return 0;
    }
    else if (strcmp(argv[1], "match"))
        return 2;
    CHECK(UmiFullQuantityEvaluate(&policy, &observation, 10000U, &review) == UMI_STATUS_OK);
    CHECK(review.assessment == expected);
    CHECK(review.displayedRuleMet == (expected == UMI_LIQUIDITY_MEETS_DISPLAYED_RULE));
    return 0;
}
