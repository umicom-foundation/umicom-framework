/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy/version_readiness.c
 *
 * PURPOSE:
 *   Apply the shared Strategy Version Readiness research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/strategy/version_readiness.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_strategy_version_readiness(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "strategy.version-readiness",
        "Strategy Version Readiness",
        UMI_STRATEGY_POLICY_READINESS,
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
