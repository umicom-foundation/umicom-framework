/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/depth/ladder_density.c
 *
 * PURPOSE:
 *   Apply the shared Depth Ladder Density calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/depth/ladder_density.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_depth_ladder_density(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "depth.ladder-density",
        "Depth Ladder Density",
        UMI_TRADING_POLICY_RATIO,
        60.000000,
        40.000000,
        UINT32_C(64),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
