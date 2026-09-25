/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/account/balance_consistency.c
 *
 * PURPOSE:
 *   Apply Account Balance Consistency without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/account/balance_consistency.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_account_balance_consistency(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "account.balance-consistency",
        "Account Balance Consistency",
        UMI_FINANCIAL_POLICY_BALANCE,
        100.000000,
        99.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
