/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/watchlist/ranking.c
 *
 * PURPOSE:
 *   Apply the shared Watchlist Ranking calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/watchlist/ranking.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_watchlist_ranking(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "watchlist.ranking",
        "Watchlist Ranking",
        UMI_TRADING_POLICY_SCORE,
        65.000000,
        50.000000,
        UINT32_C(17),
        0.300,
        0.250,
        0.250,
        0.200
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}
