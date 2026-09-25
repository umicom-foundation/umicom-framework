/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/currency/rate_freshness.c
 *
 * PURPOSE:
 *   Apply Exchange Rate Freshness without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/currency/rate_freshness.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_currency_rate_freshness(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "currency.rate-freshness",
        "Exchange Rate Freshness",
        UMI_FINANCIAL_POLICY_FRESHNESS,
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
