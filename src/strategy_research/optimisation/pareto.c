/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/optimisation/pareto.c
 *
 * PURPOSE:
 *   Apply the shared Pareto Fitness research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/optimisation/pareto.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_optimisation_pareto(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "optimisation.pareto",
        "Pareto Fitness",
        UMI_STRATEGY_POLICY_PARETO,
        60.000000,
        45.000000,
        0.300,
        0.200,
        0.300,
        0.200
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
