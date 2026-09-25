/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/currency/catalogue_coverage.h
 *
 * PURPOSE:
 *   Evaluate Currency Catalogue Coverage through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_CURRENCY_CATALOGUE_COVERAGE_H
#define UMICOM_FINANCIAL_PLATFORM_CURRENCY_CATALOGUE_COVERAGE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_currency_catalogue_coverage(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
