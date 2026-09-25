/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/chart_tape/volatility.c
 *
 * PURPOSE:
 *   Apply the shared Chart Volatility calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/chart_tape/volatility.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_chart_tape_volatility(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "chart_tape.volatility",
        "Chart Volatility",
        UMI_TRADING_POLICY_VOLATILITY,
        0.000000,
        0.000000,
        UINT32_C(128),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
