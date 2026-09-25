/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/currency/rate_tolerance.h
 *
 * PURPOSE:
 *   Evaluate Exchange Rate Tolerance through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_CURRENCY_RATE_TOLERANCE_H
#define UMICOM_FINANCIAL_PLATFORM_CURRENCY_RATE_TOLERANCE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_currency_rate_tolerance(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
