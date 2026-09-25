/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy/signal_confidence.c
 *
 * PURPOSE:
 *   Apply the shared Strategy Signal Confidence research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/strategy/signal_confidence.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_strategy_signal_confidence(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "strategy.signal-confidence",
        "Strategy Signal Confidence",
        UMI_STRATEGY_POLICY_CONFIDENCE,
        60.000000,
        45.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
