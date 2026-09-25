/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy/risk_reward.c
 *
 * PURPOSE:
 *   Apply the shared Strategy Risk Reward research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/strategy/risk_reward.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_strategy_risk_reward(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "strategy.risk-reward",
        "Strategy Risk Reward",
        UMI_STRATEGY_POLICY_RISK_REWARD,
        1.500000,
        1.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
