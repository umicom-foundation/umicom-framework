/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/performance/trade_efficiency.c
 *
 * PURPOSE:
 *   Apply the shared Trade Efficiency research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/performance/trade_efficiency.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_performance_trade_efficiency(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "performance.trade-efficiency",
        "Trade Efficiency",
        UMI_STRATEGY_POLICY_EFFICIENCY,
        60.000000,
        40.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
