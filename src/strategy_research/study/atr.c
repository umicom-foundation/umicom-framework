/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study/atr.c
 *
 * PURPOSE:
 *   Apply the shared Average True Range research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study/atr.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_study_atr(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "study.atr",
        "Average True Range",
        UMI_STRATEGY_POLICY_ATR,
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
