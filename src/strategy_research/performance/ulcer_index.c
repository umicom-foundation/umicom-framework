/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/performance/ulcer_index.c
 *
 * PURPOSE:
 *   Apply the shared Ulcer Index research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/performance/ulcer_index.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_performance_ulcer_index(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "performance.ulcer-index",
        "Ulcer Index",
        UMI_STRATEGY_POLICY_ULCER,
        0.000000,
        15.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
