/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/market/range_position.c
 *
 * PURPOSE:
 *   Apply the shared Range Position calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/market/range_position.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_market_range_position(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "market.range-position",
        "Range Position",
        UMI_TRADING_POLICY_RANGE_POSITION,
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
