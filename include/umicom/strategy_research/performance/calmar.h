/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/performance/calmar.h
 *
 * PURPOSE:
 *   Evaluate Calmar Ratio as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_PERFORMANCE_CALMAR_H
#define UMICOM_STRATEGY_RESEARCH_PERFORMANCE_CALMAR_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_performance_calmar(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
