/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/replay/checkpoint.c
 *
 * PURPOSE:
 *   Apply the shared Replay Checkpoint Readiness research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/replay/checkpoint.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_replay_checkpoint(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "replay.checkpoint",
        "Replay Checkpoint Readiness",
        UMI_STRATEGY_POLICY_CHECKPOINT,
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
