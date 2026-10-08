/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/fill_policy.c
 * PURPOSE: Evaluate full-quantity watch conditions using exact quantities and independent field freshness.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/fill_policy.h"

UmiStatus UmiFullQuantityPolicyValidate(const UmiFullQuantityPolicy *policy)
{
    if (policy == NULL ||
        (policy->mode != UMI_FULL_QUANTITY_IMMEDIATE && policy->mode != UMI_FULL_QUANTITY_WAIT &&
         policy->mode != UMI_FULL_QUANTITY_WATCH_THEN_IMMEDIATE) ||
        (policy->side != UMI_SIDE_BUY && policy->side != UMI_SIDE_SELL) ||
        (policy->waitingTimeInForce != UMI_TIF_DAY && policy->waitingTimeInForce != UMI_TIF_GTC) ||
        policy->quantity.scale > 9U || policy->quantity.coefficient <= 0 || policy->limitPrice.scale > 9U ||
        policy->limitPrice.coefficient <= 0 || policy->extraVisibleQuantity.scale > 9U ||
        policy->extraVisibleQuantity.coefficient < 0 || policy->maximumAgeMilliseconds == 0U ||
        policy->maximumSkewMilliseconds > policy->maximumAgeMilliseconds)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDecimal required;
    return UmiDecimalAddExact(policy->quantity, policy->extraVisibleQuantity, &required);
}

UmiStatus UmiFullQuantityEvaluate(const UmiFullQuantityPolicy *policy,
                                  const UmiLiquidityObservation *observation, uint64_t nowMilliseconds,
                                  UmiFullQuantityReview *out)
{
    if (observation == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiFullQuantityPolicyValidate(policy);
    if (status != UMI_STATUS_OK)
        return status;
    UmiFullQuantityReview result = {0};
    status =
        UmiDecimalAddExact(policy->quantity, policy->extraVisibleQuantity, &result.requiredVisibleQuantity);
    if (status != UMI_STATUS_OK)
        return status;
    if (!observation->available)
        result.assessment = UMI_LIQUIDITY_UNAVAILABLE;
    else
    {
        if (observation->price.scale > 9U || observation->price.coefficient <= 0 ||
            observation->visibleQuantity.scale > 9U || observation->visibleQuantity.coefficient < 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        /* Subtract only after ordering unsigned timestamps. A clock reset must
         * retire evidence, never turn it into a huge or apparently fresh age. */
        if (observation->priceReceivedAtMilliseconds > nowMilliseconds ||
            observation->sizeReceivedAtMilliseconds > nowMilliseconds)
            result.assessment = UMI_LIQUIDITY_CLOCK_MISMATCH;
        else
        {
            result.priceAgeMilliseconds = nowMilliseconds - observation->priceReceivedAtMilliseconds;
            result.sizeAgeMilliseconds = nowMilliseconds - observation->sizeReceivedAtMilliseconds;
            const uint64_t skew = result.priceAgeMilliseconds > result.sizeAgeMilliseconds
                                      ? result.priceAgeMilliseconds - result.sizeAgeMilliseconds
                                      : result.sizeAgeMilliseconds - result.priceAgeMilliseconds;
            int priceComparison = 0, sizeComparison = 0;
            (void)UmiDecimalCompare(observation->price, policy->limitPrice, &priceComparison);
            (void)UmiDecimalCompare(observation->visibleQuantity, result.requiredVisibleQuantity,
                                    &sizeComparison);
            if (!observation->realtime)
                result.assessment = UMI_LIQUIDITY_NOT_REALTIME;
            else if (observation->stale || result.priceAgeMilliseconds > policy->maximumAgeMilliseconds ||
                     result.sizeAgeMilliseconds > policy->maximumAgeMilliseconds)
                result.assessment = UMI_LIQUIDITY_STALE;
            else if (skew > policy->maximumSkewMilliseconds)
                result.assessment = UMI_LIQUIDITY_FIELDS_SKEWED;
            else if ((policy->side == UMI_SIDE_BUY && priceComparison > 0) ||
                     (policy->side == UMI_SIDE_SELL && priceComparison < 0))
                result.assessment = UMI_LIQUIDITY_OUTSIDE_LIMIT;
            else if (sizeComparison < 0)
                result.assessment = UMI_LIQUIDITY_INSUFFICIENT;
            else
            {
                result.assessment = UMI_LIQUIDITY_MEETS_DISPLAYED_RULE;
                result.displayedRuleMet = true;
            }
        }
    }
    *out = result;
    return UMI_STATUS_OK;
}

const char *UmiLiquidityAssessmentText(UmiLiquidityAssessment assessment)
{
    switch (assessment)
    {
    case UMI_LIQUIDITY_UNAVAILABLE:
        return "Waiting for both price and displayed size.";
    case UMI_LIQUIDITY_NOT_REALTIME:
        return "Delayed, frozen or unknown data cannot satisfy this watch.";
    case UMI_LIQUIDITY_STALE:
        return "Price or size evidence is stale.";
    case UMI_LIQUIDITY_CLOCK_MISMATCH:
        return "Receipt time is ahead of the review clock; refresh the observation.";
    case UMI_LIQUIDITY_FIELDS_SKEWED:
        return "Price and size receipt times are too far apart.";
    case UMI_LIQUIDITY_OUTSIDE_LIMIT:
        return "The observed price is outside your limit.";
    case UMI_LIQUIDITY_INSUFFICIENT:
        return "Displayed size is below your quantity plus buffer.";
    case UMI_LIQUIDITY_MEETS_DISPLAYED_RULE:
        return "Displayed price and size meet your rule; execution is not guaranteed.";
    default:
        return "Unknown liquidity assessment.";
    }
}
