/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/pricing_engine.c
 *
 * PURPOSE:
 *   Implement deterministic FX-forward and discount calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/pricing_engine.h"

#include <stddef.h>

UmiStatus umi_open_tms_price_fx_forward(
    double spot,
    double domesticRate,
    double foreignRate,
    double yearFraction,
    double *outForward)
{
    double denominator;
    if (outForward == NULL || spot <= 0.0 || yearFraction < 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    denominator = 1.0 + foreignRate * yearFraction;
    if (denominator <= 0.0) return UMI_STATUS_INVALID_STATE;
    *outForward =
        spot * (1.0 + domesticRate * yearFraction) / denominator;
    return UMI_STATUS_OK;
}

UmiStatus umi_open_tms_discount_factor(
    double rate,
    double yearFraction,
    double *outDiscount)
{
    double denominator;
    if (outDiscount == NULL || yearFraction < 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    denominator = 1.0 + rate * yearFraction;
    if (denominator <= 0.0) return UMI_STATUS_INVALID_STATE;
    *outDiscount = 1.0 / denominator;
    return UMI_STATUS_OK;
}

UmiStatus umi_open_tms_present_value(
    double futureAmount,
    double discountFactor,
    double *outPresentValue)
{
    if (outPresentValue == NULL || discountFactor <= 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outPresentValue = futureAmount * discountFactor;
    return UMI_STATUS_OK;
}
