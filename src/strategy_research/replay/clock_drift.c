/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/replay/clock_drift.c
 *
 * PURPOSE:
 *   Apply the shared Replay Clock Drift research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/replay/clock_drift.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_replay_clock_drift(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "replay.clock-drift",
        "Replay Clock Drift",
        UMI_STRATEGY_POLICY_CLOCK_DRIFT,
        0.000000,
        1000.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
