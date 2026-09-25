/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/currency/conversion_consistency.h
 *
 * PURPOSE:
 *   Evaluate FX Conversion Consistency through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_CURRENCY_CONVERSION_CONSISTENCY_H
#define UMICOM_FINANCIAL_PLATFORM_CURRENCY_CONVERSION_CONSISTENCY_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_currency_conversion_consistency(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
