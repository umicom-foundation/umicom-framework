/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/settlement/cash_leg.c
 *
 * PURPOSE:
 *   Apply Settlement Cash Leg without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/settlement/cash_leg.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_settlement_cash_leg(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "settlement.cash-leg",
        "Settlement Cash Leg",
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
