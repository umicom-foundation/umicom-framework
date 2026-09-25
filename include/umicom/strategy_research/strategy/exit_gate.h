/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/strategy/exit_gate.h
 *
 * PURPOSE:
 *   Evaluate Strategy Exit Gate as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_STRATEGY_EXIT_GATE_H
#define UMICOM_STRATEGY_RESEARCH_STRATEGY_EXIT_GATE_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_strategy_exit_gate(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
