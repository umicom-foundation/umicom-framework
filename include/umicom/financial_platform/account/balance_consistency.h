/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/account/balance_consistency.h
 *
 * PURPOSE:
 *   Evaluate Account Balance Consistency through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_ACCOUNT_BALANCE_CONSISTENCY_H
#define UMICOM_FINANCIAL_PLATFORM_ACCOUNT_BALANCE_CONSISTENCY_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_account_balance_consistency(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
