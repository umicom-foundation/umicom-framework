/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study/rsi.c
 *
 * PURPOSE:
 *   Apply the shared Relative Strength Index research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study/rsi.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_study_rsi(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "study.rsi",
        "Relative Strength Index",
        UMI_STRATEGY_POLICY_RSI,
        0.000000,
        70.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
