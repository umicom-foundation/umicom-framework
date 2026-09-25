/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/market/feed_health.c
 *
 * PURPOSE:
 *   Apply the shared Market Feed Health calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/market/feed_health.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_market_feed_health(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "market.feed-health",
        "Market Feed Health",
        UMI_TRADING_POLICY_HEALTH,
        75.000000,
        50.000000,
        UINT32_C(9),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
