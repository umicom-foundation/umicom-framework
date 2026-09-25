/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/optimisation/sensitivity.h
 *
 * PURPOSE:
 *   Evaluate Parameter Sensitivity as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_OPTIMISATION_SENSITIVITY_H
#define UMICOM_STRATEGY_RESEARCH_OPTIMISATION_SENSITIVITY_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_optimisation_sensitivity(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
