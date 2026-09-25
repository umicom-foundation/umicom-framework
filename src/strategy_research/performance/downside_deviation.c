/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/performance/downside_deviation.c
 *
 * PURPOSE:
 *   Apply the shared Downside Deviation research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/performance/downside_deviation.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_performance_downside_deviation(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "performance.downside-deviation",
        "Downside Deviation",
        UMI_STRATEGY_POLICY_DOWNSIDE_DEV,
        0.000000,
        25.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
