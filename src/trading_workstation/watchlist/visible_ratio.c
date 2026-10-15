/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/watchlist/visible_ratio.c
 *
 * PURPOSE:
 *   Apply the shared Watchlist Visible Ratio calculation over copied market/workspace
 *   evidence without mutating trading, risk, execution or linked-context state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/trading_workstation/watchlist/visible_ratio.h"
#include "../trading_workstation_internal.h"

UmiStatus umi_trading_professional_evaluate_watchlist_visible_ratio(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    static const UmiTradingProfessionalPolicy policy = {
        "watchlist.visible-ratio",
        "Watchlist Visible Ratio",
        UMI_TRADING_POLICY_RATIO,
        60.000000,
        40.000000,
        UINT32_C(1),
        0.250,
        0.250,
        0.250,
        0.250
    };

    return umi_trading_professional_evaluate_policy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_trading_professional_evaluate_watchlist_visible_ratio_batch, umi_trading_professional_evaluate_watchlist_visible_ratio,
    UmiTradingProfessionalInput, UmiTradingProfessionalSnapshot)
