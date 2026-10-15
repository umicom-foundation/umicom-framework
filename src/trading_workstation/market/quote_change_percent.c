/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/market/quote_change_percent.c
 *
 * PURPOSE:
 *   Apply the shared Quote Change Percent calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/trading_workstation/market/quote_change_percent.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_market_quote_change_percent(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "market.quote-change-percent",
        "Quote Change Percent",
        UMI_TRADING_POLICY_CHANGE_PERCENT,
        0.000000,
        0.000000,
        UINT32_C(32),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_market_quote_change_percent_batch, umi_trading_professional_evaluate_market_quote_change_percent,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
