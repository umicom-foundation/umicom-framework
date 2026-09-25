/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/depth/top_liquidity.c
 *
 * PURPOSE:
 *   Apply the shared Top Liquidity calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/depth/top_liquidity.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_depth_top_liquidity(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "depth.top-liquidity",
        "Top Liquidity",
        UMI_TRADING_POLICY_LIQUIDITY,
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
