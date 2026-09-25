/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/ledger/suspense.h
 *
 * PURPOSE:
 *   Evaluate Suspense Utilisation through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_LEDGER_SUSPENSE_H
#define UMICOM_FINANCIAL_PLATFORM_LEDGER_SUSPENSE_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_ledger_suspense(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
