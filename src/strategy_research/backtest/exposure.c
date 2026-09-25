/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest/exposure.c
 *
 * PURPOSE:
 *   Apply the shared Backtest Exposure research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/backtest/exposure.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_backtest_exposure(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "backtest.exposure",
        "Backtest Exposure",
        UMI_STRATEGY_POLICY_EXPOSURE,
        0.000000,
        85.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
