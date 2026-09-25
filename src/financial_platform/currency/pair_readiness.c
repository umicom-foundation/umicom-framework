/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/currency/pair_readiness.c
 *
 * PURPOSE:
 *   Apply Currency Pair Readiness without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/currency/pair_readiness.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_currency_pair_readiness(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "currency.pair-readiness",
        "Currency Pair Readiness",
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
