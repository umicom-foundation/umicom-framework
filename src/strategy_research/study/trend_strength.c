/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study/trend_strength.c
 *
 * PURPOSE:
 *   Apply the shared Trend Strength research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study/trend_strength.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_study_trend_strength(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "study.trend-strength",
        "Trend Strength",
        UMI_STRATEGY_POLICY_TREND,
        60.000000,
        40.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
