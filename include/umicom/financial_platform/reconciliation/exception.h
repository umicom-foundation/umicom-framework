/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/reconciliation/exception.h
 *
 * PURPOSE:
 *   Evaluate Reconciliation Exception Approval through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_EXCEPTION_H
#define UMICOM_FINANCIAL_PLATFORM_RECONCILIATION_EXCEPTION_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_reconciliation_exception(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
