/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/market/quote_midprice.c
 *
 * PURPOSE:
 *   Apply the shared Quote Midprice calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/market/quote_midprice.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_market_quote_midprice(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "market.quote-midprice",
        "Quote Midprice",
        UMI_TRADING_POLICY_MIDPRICE,
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
