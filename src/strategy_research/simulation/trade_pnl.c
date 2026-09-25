/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/simulation/trade_pnl.c
 *
 * PURPOSE:
 *   Apply the shared Simulation Trade P&L research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/simulation/trade_pnl.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_simulation_trade_pnl(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "simulation.trade-pnl",
        "Simulation Trade P&L",
        UMI_STRATEGY_POLICY_TRADE_PNL,
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
