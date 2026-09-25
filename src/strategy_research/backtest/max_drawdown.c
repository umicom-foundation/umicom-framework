/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest/max_drawdown.c
 *
 * PURPOSE:
 *   Apply the shared Backtest Maximum Drawdown research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/backtest/max_drawdown.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_backtest_max_drawdown(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "backtest.max-drawdown",
        "Backtest Maximum Drawdown",
        UMI_STRATEGY_POLICY_MAX_DRAWDOWN,
        0.000000,
        20.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
