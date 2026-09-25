/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/chart_tape/chart_readiness.c
 *
 * PURPOSE:
 *   Apply the shared Chart Readiness calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/chart_tape/chart_readiness.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_chart_tape_chart_readiness(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "chart_tape.chart-readiness",
        "Chart Readiness",
        UMI_TRADING_POLICY_READINESS,
        0.000000,
        0.000000,
        UINT32_C(145),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
