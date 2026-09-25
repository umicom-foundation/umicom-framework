/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/reconciliation/tolerance.h
 *
 * PURPOSE:
 *   Evaluate Reconciliation Tolerance through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_TOLERANCE_H
#define UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_TOLERANCE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_reconciliation_tolerance(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
