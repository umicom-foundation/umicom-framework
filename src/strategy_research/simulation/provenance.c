/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/simulation/provenance.c
 *
 * PURPOSE:
 *   Apply the shared Simulation Provenance research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/simulation/provenance.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_simulation_provenance(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "simulation.provenance",
        "Simulation Provenance",
        UMI_STRATEGY_POLICY_PROVENANCE,
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
