/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/account/available_funds.h
 *
 * PURPOSE:
 *   Evaluate Available Funds through the shared financial platform control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_ACCOUNT_AVAILABLE_FUNDS_H
#define UMICOM_FINANCIAL_PLATFORM_ACCOUNT_AVAILABLE_FUNDS_H

#include "umicom/financial_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_financial_platform_evaluate_account_available_funds(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
