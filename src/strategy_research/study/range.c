/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study/range.c
 *
 * PURPOSE:
 *   Apply the shared Price Range research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study/range.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_study_range(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "study.range",
        "Price Range",
        UMI_STRATEGY_POLICY_RANGE,
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
