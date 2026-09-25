/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/replay/gap_ratio.c
 *
 * PURPOSE:
 *   Apply the shared Replay Gap Ratio research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/replay/gap_ratio.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_replay_gap_ratio(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "replay.gap-ratio",
        "Replay Gap Ratio",
        UMI_STRATEGY_POLICY_GAP_RATIO,
        0.000000,
        2.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
