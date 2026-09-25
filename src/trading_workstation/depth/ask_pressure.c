/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/depth/ask_pressure.c
 *
 * PURPOSE:
 *   Apply the shared Ask Pressure calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/depth/ask_pressure.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_depth_ask_pressure(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "depth.ask-pressure",
        "Ask Pressure",
        UMI_TRADING_POLICY_PRESSURE,
        0.000000,
        0.000000,
        UINT32_C(64),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
