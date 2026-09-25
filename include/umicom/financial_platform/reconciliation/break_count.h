/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/reconciliation/break_count.h
 *
 * PURPOSE:
 *   Evaluate Reconciliation Break Count through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_BREAK_COUNT_H
#define UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_BREAK_COUNT_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_reconciliation_break_count(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
