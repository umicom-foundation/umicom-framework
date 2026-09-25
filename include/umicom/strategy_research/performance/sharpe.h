/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/performance/sharpe.h
 *
 * PURPOSE:
 *   Evaluate Sharpe Ratio as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_PERFORMANCE_SHARPE_H
#define UMICOM_STRATEGY_RESEARCH_PERFORMANCE_SHARPE_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_performance_sharpe(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
