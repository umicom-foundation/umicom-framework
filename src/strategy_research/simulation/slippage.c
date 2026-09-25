/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/simulation/slippage.c
 *
 * PURPOSE:
 *   Apply the shared Simulation Slippage research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/simulation/slippage.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_simulation_slippage(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "simulation.slippage",
        "Simulation Slippage",
        UMI_STRATEGY_POLICY_SLIPPAGE,
        0.000000,
        10.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_strategy_research_evaluate_policy(
        &policy, input, outSnapshot);
}
