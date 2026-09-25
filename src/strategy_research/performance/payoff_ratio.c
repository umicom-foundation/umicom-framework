/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/performance/payoff_ratio.c
 *
 * PURPOSE:
 *   Apply the shared Payoff Ratio research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/performance/payoff_ratio.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_performance_payoff_ratio(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "performance.payoff-ratio",
        "Payoff Ratio",
        UMI_STRATEGY_POLICY_PAYOFF,
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
