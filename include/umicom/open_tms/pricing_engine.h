/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/pricing_engine.h
 *
 * PURPOSE:
 *   Provide deterministic treasury FX-forward and discounted-cashflow pricing helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_PRICING_ENGINE_H
#define UMICOM_OPEN_TMS_PRICING_ENGINE_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_price_fx_forward(
    double spot,
    double domesticRate,
    double foreignRate,
    double yearFraction,
    double *outForward);

UmiStatus umi_open_tms_discount_factor(
    double rate,
    double yearFraction,
    double *outDiscount);

UmiStatus umi_open_tms_present_value(
    double futureAmount,
    double discountFactor,
    double *outPresentValue);

#ifdef __cplusplus
}
#endif
#endif
