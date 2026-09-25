/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/market/spread_basis_points.c
 *
 * PURPOSE:
 *   Apply the shared Spread Basis Points calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/market/spread_basis_points.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_market_spread_basis_points(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "market.spread-basis-points",
        "Spread Basis Points",
        UMI_TRADING_POLICY_SPREAD_BPS,
        0.000000,
        35.000000,
        UINT32_C(32),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
