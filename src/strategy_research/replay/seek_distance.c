/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/replay/seek_distance.c
 *
 * PURPOSE:
 *   Apply the shared Replay Seek Distance research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/replay/seek_distance.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_replay_seek_distance(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "replay.seek-distance",
        "Replay Seek Distance",
        UMI_STRATEGY_POLICY_SEEK_DISTANCE,
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
