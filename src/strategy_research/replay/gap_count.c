/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/replay/gap_count.c
 *
 * PURPOSE:
 *   Apply the shared Replay Gap Count research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/replay/gap_count.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_replay_gap_count(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "replay.gap-count",
        "Replay Gap Count",
        UMI_STRATEGY_POLICY_GAP_COUNT,
        0.000000,
        1.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
