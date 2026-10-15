/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/trade/lifecycle.c
 *
 * PURPOSE:
 *   Apply Trade Lifecycle without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/financial_platform/trade/lifecycle.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_trade_lifecycle(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "trade.lifecycle",
        "Trade Lifecycle",
        UMI_FINANCIAL_POLICY_TRANSITION,
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
UMI_DEFINE_POLICY_BATCH(umi_financial_platform_evaluate_trade_lifecycle_batch, umi_financial_platform_evaluate_trade_lifecycle,
    UmiFinancialPlatformInput, UmiFinancialPlatformSnapshot)
