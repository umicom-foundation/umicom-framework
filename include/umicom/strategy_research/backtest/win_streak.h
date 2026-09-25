/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/backtest/win_streak.h
 *
 * PURPOSE:
 *   Evaluate Backtest Win Streak as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_BACKTEST_WIN_STREAK_H
#define UMICOM_STRATEGY_RESEARCH_BACKTEST_WIN_STREAK_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_backtest_win_streak(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
