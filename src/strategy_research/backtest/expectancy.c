/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest/expectancy.c
 *
 * PURPOSE:
 *   Apply the shared Backtest Expectancy research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/backtest/expectancy.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_backtest_expectancy(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "backtest.expectancy",
        "Backtest Expectancy",
        UMI_STRATEGY_POLICY_EXPECTANCY,
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
