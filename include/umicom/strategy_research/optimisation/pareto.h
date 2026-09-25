/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/optimisation/pareto.h
 *
 * PURPOSE:
 *   Evaluate Pareto Fitness as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_OPTIMISATION_PARETO_H
#define UMICOM_STRATEGY_RESEARCH_OPTIMISATION_PARETO_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_optimisation_pareto(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
