/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/optimisation/budget.c
 *
 * PURPOSE:
 *   Apply the shared Optimisation Budget research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/optimisation/budget.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_optimisation_budget(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "optimisation.budget",
        "Optimisation Budget",
        UMI_STRATEGY_POLICY_BUDGET,
        0.000000,
        90.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
