/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/reconciliation/coverage.h
 *
 * PURPOSE:
 *   Evaluate Reconciliation Coverage through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_COVERAGE_H
#define UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_COVERAGE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_reconciliation_coverage(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
