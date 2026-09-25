/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/study/crossover.h
 *
 * PURPOSE:
 *   Evaluate Moving Average Crossover as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_STUDY_CROSSOVER_H
#define UMICOM_STRATEGY_RESEARCH_STUDY_CROSSOVER_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_study_crossover(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
