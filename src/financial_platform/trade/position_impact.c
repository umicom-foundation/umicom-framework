/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/trade/position_impact.c
 *
 * PURPOSE:
 *   Apply Trade Position Impact without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/trade/position_impact.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_trade_position_impact(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "trade.position-impact",
        "Trade Position Impact",
        UMI_FINANCIAL_POLICY_CONSISTENCY,
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
