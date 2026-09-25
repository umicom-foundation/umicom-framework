/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/performance/annualised_volatility.c
 *
 * PURPOSE:
 *   Apply the shared Annualised Volatility research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/performance/annualised_volatility.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_performance_annualised_volatility(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "performance.annualised-volatility",
        "Annualised Volatility",
        UMI_STRATEGY_POLICY_ANNUALISED_VOL,
        0.000000,
        40.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
