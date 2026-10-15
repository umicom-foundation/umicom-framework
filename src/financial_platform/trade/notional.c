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
#include "../../base/policy_batch_internal.h"
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_financial_platform_evaluate_trade_notional_batch, umi_financial_platform_evaluate_trade_notional,
    UmiFinancialPlatformInput, UmiFinancialPlatformSnapshot)
