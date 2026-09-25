/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/simulation/fill_ratio.c
 *
 * PURPOSE:
 *   Apply the shared Simulation Fill Ratio research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/simulation/fill_ratio.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_simulation_fill_ratio(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "simulation.fill-ratio",
        "Simulation Fill Ratio",
        UMI_STRATEGY_POLICY_FILL_RATIO,
        95.000000,
        80.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
