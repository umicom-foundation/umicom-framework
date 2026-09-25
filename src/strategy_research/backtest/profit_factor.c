/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest/profit_factor.c
 *
 * PURPOSE:
 *   Apply the shared Backtest Profit Factor research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/backtest/profit_factor.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_backtest_profit_factor(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "backtest.profit-factor",
        "Backtest Profit Factor",
        UMI_STRATEGY_POLICY_PROFIT_FACTOR,
        1.200000,
        1.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
