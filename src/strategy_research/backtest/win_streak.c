/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest/win_streak.c
 *
 * PURPOSE:
 *   Apply the shared Backtest Win Streak research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/strategy_research/backtest/win_streak.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_backtest_win_streak(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "backtest.win-streak",
        "Backtest Win Streak",
        UMI_STRATEGY_POLICY_STREAK,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_strategy_research_evaluate_backtest_win_streak_batch, umi_strategy_research_evaluate_backtest_win_streak,
    UmiStrategyResearchInput, UmiStrategyResearchSnapshot)
