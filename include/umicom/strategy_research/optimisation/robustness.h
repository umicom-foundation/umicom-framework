/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/optimisation/robustness.h
 *
 * PURPOSE:
 *   Evaluate Parameter Robustness as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_OPTIMISATION_ROBUSTNESS_H
#define UMICOM_STRATEGY_RESEARCH_OPTIMISATION_ROBUSTNESS_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_optimisation_robustness(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
