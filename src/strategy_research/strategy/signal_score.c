/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy/signal_score.c
 *
 * PURPOSE:
 *   Apply the shared Strategy Signal Score research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/strategy/signal_score.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_strategy_signal_score(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "strategy.signal-score",
        "Strategy Signal Score",
        UMI_STRATEGY_POLICY_SCORE,
        60.000000,
        45.000000,
        0.350,
        0.250,
        0.250,
        0.150
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
