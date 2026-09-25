/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study/volatility.c
 *
 * PURPOSE:
 *   Apply the shared Rolling Volatility research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study/volatility.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_study_volatility(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "study.volatility",
        "Rolling Volatility",
        UMI_STRATEGY_POLICY_VOLATILITY,
        0.000000,
        40.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
