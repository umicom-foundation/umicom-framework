/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study/volume_ratio.c
 *
 * PURPOSE:
 *   Apply the shared Volume Ratio research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study/volume_ratio.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_study_volume_ratio(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "study.volume-ratio",
        "Volume Ratio",
        UMI_STRATEGY_POLICY_VOLUME_RATIO,
        100.000000,
        60.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
