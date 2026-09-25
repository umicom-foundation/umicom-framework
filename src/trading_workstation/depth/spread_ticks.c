/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/depth/spread_ticks.c
 *
 * PURPOSE:
 *   Apply the shared Depth Spread Ticks calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/depth/spread_ticks.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_depth_spread_ticks(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "depth.spread-ticks",
        "Depth Spread Ticks",
        UMI_TRADING_POLICY_SPREAD_TICKS,
        0.000000,
        6.000000,
        UINT32_C(96),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
