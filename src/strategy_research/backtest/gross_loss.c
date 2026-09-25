/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest/gross_loss.c
 *
 * PURPOSE:
 *   Apply the shared Backtest Gross Loss research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/backtest/gross_loss.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_backtest_gross_loss(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "backtest.gross-loss",
        "Backtest Gross Loss",
        UMI_STRATEGY_POLICY_GROSS_LOSS,
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
