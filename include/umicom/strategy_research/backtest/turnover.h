/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/backtest/turnover.h
 *
 * PURPOSE:
 *   Evaluate Backtest Turnover as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_BACKTEST_TURNOVER_H
#define UMICOM_STRATEGY_RESEARCH_BACKTEST_TURNOVER_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_backtest_turnover(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
