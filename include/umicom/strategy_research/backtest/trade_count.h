/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/backtest/trade_count.h
 *
 * PURPOSE:
 *   Evaluate Backtest Trade Count as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_BACKTEST_TRADE_COUNT_H
#define UMICOM_STRATEGY_RESEARCH_BACKTEST_TRADE_COUNT_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_backtest_trade_count(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
