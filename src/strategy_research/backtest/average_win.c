/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest/average_win.c
 *
 * PURPOSE:
 *   Apply the shared Backtest Average Win research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/backtest/average_win.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_backtest_average_win(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "backtest.average-win",
        "Backtest Average Win",
        UMI_STRATEGY_POLICY_AVG_WIN,
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
