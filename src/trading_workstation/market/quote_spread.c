/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/market/quote_spread.c
 *
 * PURPOSE:
 *   Apply the shared Quote Spread calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/market/quote_spread.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_market_quote_spread(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "market.quote-spread",
        "Quote Spread",
        UMI_TRADING_POLICY_SPREAD,
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
