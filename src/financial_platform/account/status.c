/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/account/status.c
 *
 * PURPOSE:
 *   Apply Account Status without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/account/status.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_account_status(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "account.status",
        "Account Status",
        UMI_FINANCIAL_POLICY_READINESS,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
