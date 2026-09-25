/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/depth/sweep_risk.c
 *
 * PURPOSE:
 *   Apply the shared Liquidity Sweep Risk calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/depth/sweep_risk.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_depth_sweep_risk(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "depth.sweep-risk",
        "Liquidity Sweep Risk",
        UMI_TRADING_POLICY_SCORE,
        70.000000,
        50.000000,
        UINT32_C(64),
        0.150,
        0.150,
        0.450,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
