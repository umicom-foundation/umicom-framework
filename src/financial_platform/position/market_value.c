/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/position/market_value.c
 *
 * PURPOSE:
 *   Apply Position Market Value without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/position/market_value.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_position_market_value(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "position.market-value",
        "Position Market Value",
        UMI_FINANCIAL_POLICY_AMOUNT,
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
