/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study/rate_of_change.c
 *
 * PURPOSE:
 *   Apply the shared Rate of Change research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study/rate_of_change.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_study_rate_of_change(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "study.rate-of-change",
        "Rate of Change",
        UMI_STRATEGY_POLICY_ROC,
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
