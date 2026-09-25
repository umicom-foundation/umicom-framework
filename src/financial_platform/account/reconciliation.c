/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/account/reconciliation.c
 *
 * PURPOSE:
 *   Apply Account Reconciliation without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/account/reconciliation.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_account_reconciliation(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "account.reconciliation",
        "Account Reconciliation",
        UMI_FINANCIAL_POLICY_MATCH,
        100.000000,
        95.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
