/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/optimisation/walk_forward.h
 *
 * PURPOSE:
 *   Evaluate Walk Forward Score as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_OPTIMISATION_WALK_FORWARD_H
#define UMICOM_STRATEGY_RESEARCH_OPTIMISATION_WALK_FORWARD_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_optimisation_walk_forward(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
