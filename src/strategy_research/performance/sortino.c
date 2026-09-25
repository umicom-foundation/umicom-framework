/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/performance/sortino.c
 *
 * PURPOSE:
 *   Apply the shared Sortino Ratio research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/performance/sortino.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_performance_sortino(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "performance.sortino",
        "Sortino Ratio",
        UMI_STRATEGY_POLICY_SORTINO,
        1.000000,
        0.500000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
