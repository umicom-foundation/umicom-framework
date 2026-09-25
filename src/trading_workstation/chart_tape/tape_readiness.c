/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/chart_tape/tape_readiness.c
 *
 * PURPOSE:
 *   Apply the shared Time and Sales Readiness calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/chart_tape/tape_readiness.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_chart_tape_tape_readiness(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "chart_tape.tape-readiness",
        "Time and Sales Readiness",
        UMI_TRADING_POLICY_READINESS,
        0.000000,
        0.000000,
        UINT32_C(273),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
