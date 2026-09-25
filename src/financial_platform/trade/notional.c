/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/trade/notional.c
 *
 * PURPOSE:
 *   Apply Trade Notional without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/trade/notional.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_trade_notional(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "trade.notional",
        "Trade Notional",
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
