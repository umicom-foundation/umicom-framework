/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/currency/precision_validation.h
 *
 * PURPOSE:
 *   Evaluate Currency Precision Validation through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_CURRENCY_PRECISION_VALIDATION_H
#define UMICOM_FINANCIAL_PLATFORM_CURRENCY_PRECISION_VALIDATION_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_currency_precision_validation(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
