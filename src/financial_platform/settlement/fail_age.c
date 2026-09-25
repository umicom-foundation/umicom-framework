/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/settlement/fail_age.c
 *
 * PURPOSE:
 *   Apply Settlement Fail Age without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/settlement/fail_age.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_settlement_fail_age(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "settlement.fail-age",
        "Settlement Fail Age",
        UMI_FINANCIAL_POLICY_AGEING,
        0.000000,
        86400000.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}
