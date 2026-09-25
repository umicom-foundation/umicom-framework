/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest/recovery_factor.c
 *
 * PURPOSE:
 *   Apply the shared Backtest Recovery Factor research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/backtest/recovery_factor.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_backtest_recovery_factor(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "backtest.recovery-factor",
        "Backtest Recovery Factor",
        UMI_STRATEGY_POLICY_RECOVERY_FACTOR,
        1.000000,
        0.750000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
