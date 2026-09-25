/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest/net_profit.c
 *
 * PURPOSE:
 *   Apply the shared Backtest Net Profit research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/backtest/net_profit.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_backtest_net_profit(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "backtest.net-profit",
        "Backtest Net Profit",
        UMI_STRATEGY_POLICY_NET_PROFIT,
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
