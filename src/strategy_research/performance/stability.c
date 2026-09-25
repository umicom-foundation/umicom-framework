/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/performance/stability.c
 *
 * PURPOSE:
 *   Apply the shared Equity Stability research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/performance/stability.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_performance_stability(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "performance.stability",
        "Equity Stability",
        UMI_STRATEGY_POLICY_STABILITY,
        65.000000,
        45.000000,
        0.200,
        0.300,
        0.300,
        0.200
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
