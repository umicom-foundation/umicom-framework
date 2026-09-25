/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy/signal_expiry.c
 *
 * PURPOSE:
 *   Apply the shared Strategy Signal Expiry research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/strategy/signal_expiry.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_strategy_signal_expiry(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "strategy.signal-expiry",
        "Strategy Signal Expiry",
        UMI_STRATEGY_POLICY_EXPIRY,
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
