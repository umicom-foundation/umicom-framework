/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy/position_size.c
 *
 * PURPOSE:
 *   Apply the shared Strategy Position Size research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/strategy/position_size.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_strategy_position_size(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "strategy.position-size",
        "Strategy Position Size",
        UMI_STRATEGY_POLICY_POSITION_SIZE,
        0.000000,
        80.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
