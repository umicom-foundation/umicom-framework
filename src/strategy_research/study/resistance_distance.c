/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study/resistance_distance.c
 *
 * PURPOSE:
 *   Apply the shared Resistance Distance research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study/resistance_distance.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_study_resistance_distance(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "study.resistance-distance",
        "Resistance Distance",
        UMI_STRATEGY_POLICY_RESISTANCE_DISTANCE,
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
