/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/account/available_funds.c
 *
 * PURPOSE:
 *   Apply Available Funds without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/account/available_funds.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_account_available_funds(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "account.available-funds",
        "Available Funds",
        UMI_FINANCIAL_POLICY_LIMIT,
        0.000000,
        90.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
