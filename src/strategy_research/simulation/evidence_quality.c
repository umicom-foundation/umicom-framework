/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/simulation/evidence_quality.c
 *
 * PURPOSE:
 *   Apply the shared Simulation Evidence Quality research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/simulation/evidence_quality.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_simulation_evidence_quality(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "simulation.evidence-quality",
        "Simulation Evidence Quality",
        UMI_STRATEGY_POLICY_EVIDENCE,
        95.000000,
        85.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
