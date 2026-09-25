/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/ledger/trial_balance.c
 *
 * PURPOSE:
 *   Apply Trial Balance without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/ledger/trial_balance.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_ledger_trial_balance(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "ledger.trial-balance",
        "Trial Balance",
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
