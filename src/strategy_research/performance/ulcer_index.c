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
#include "../../base/policy_batch_internal.h"
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_strategy_research_evaluate_performance_ulcer_index_batch, umi_strategy_research_evaluate_performance_ulcer_index,
    UmiStrategyResearchInput, UmiStrategyResearchSnapshot)
