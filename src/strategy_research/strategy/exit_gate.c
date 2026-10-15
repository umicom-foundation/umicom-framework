/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy/exit_gate.c
 *
 * PURPOSE:
 *   Apply the shared Strategy Exit Gate research calculation without granting live
 *   trading authority or duplicating the canonical trading/replay services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/strategy_research/strategy/exit_gate.h"
#include "../strategy_research_internal.h"

UmiStatus umi_strategy_research_evaluate_strategy_exit_gate(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot)
{
    static const UmiStrategyResearchPolicy policy = {
        "strategy.exit-gate",
        "Strategy Exit Gate",
        UMI_STRATEGY_POLICY_EXIT_GATE,
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

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_strategy_research_evaluate_strategy_exit_gate_batch, umi_strategy_research_evaluate_strategy_exit_gate,
    UmiStrategyResearchInput, UmiStrategyResearchSnapshot)
