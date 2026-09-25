/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/replay/data_quality.c
 *
 * PURPOSE:
 *   Apply the shared Replay Data Quality research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/replay/data_quality.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_replay_data_quality(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "replay.data-quality",
        "Replay Data Quality",
        UMI_STRATEGY_POLICY_DATA_QUALITY,
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
