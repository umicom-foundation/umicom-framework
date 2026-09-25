/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/optimisation/objective.c
 *
 * PURPOSE:
 *   Apply the shared Optimisation Objective research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/optimisation/objective.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_optimisation_objective(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "optimisation.objective",
        "Optimisation Objective",
        UMI_STRATEGY_POLICY_OBJECTIVE,
        60.000000,
        45.000000,
        0.300,
        0.250,
        0.250,
        0.200
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
