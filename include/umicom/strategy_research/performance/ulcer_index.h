/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/performance/ulcer_index.h
 *
 * PURPOSE:
 *   Evaluate Ulcer Index as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_PERFORMANCE_ULCER_INDEX_H
#define UMICOM_STRATEGY_RESEARCH_PERFORMANCE_ULCER_INDEX_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_performance_ulcer_index(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
