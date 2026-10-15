/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/simulation/position_change.c
 *
 * PURPOSE:
 *   Apply the shared Simulation Position Change research calculation without granting live
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
#include "umicom/strategy_research/simulation/position_change.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_simulation_position_change(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "simulation.position-change",
        "Simulation Position Change",
        UMI_STRATEGY_POLICY_POSITION_CHANGE,
        0.000000,
        0.000000,
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
UMI_DEFINE_POLICY_BATCH(umi_strategy_research_evaluate_simulation_position_change_batch, umi_strategy_research_evaluate_simulation_position_change,
    UmiStrategyResearchInput, UmiStrategyResearchSnapshot)
