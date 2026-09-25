/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study/crossover.c
 *
 * PURPOSE:
 *   Apply the shared Moving Average Crossover research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study/crossover.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_study_crossover(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "study.crossover",
        "Moving Average Crossover",
        UMI_STRATEGY_POLICY_CROSSOVER,
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
