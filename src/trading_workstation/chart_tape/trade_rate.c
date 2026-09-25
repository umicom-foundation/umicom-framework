/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/chart_tape/trade_rate.c
 *
 * PURPOSE:
 *   Apply the shared Trade Rate calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/chart_tape/trade_rate.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_chart_tape_trade_rate(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "chart_tape.trade-rate",
        "Trade Rate",
        UMI_TRADING_POLICY_RATE,
        0.000000,
        0.000000,
        UINT32_C(256),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
