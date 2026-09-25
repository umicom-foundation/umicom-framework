/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/optimisation/budget.h
 *
 * PURPOSE:
 *   Evaluate Optimisation Budget as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_OPTIMISATION_BUDGET_H
#define UMICOM_STRATEGY_RESEARCH_OPTIMISATION_BUDGET_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_optimisation_budget(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
