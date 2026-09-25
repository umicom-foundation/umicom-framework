/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/backtest_engine.h
 *
 * PURPOSE:
 *   Aggregate deterministic simulated trades into backtest P&L, drawdown,
 *   turnover, exposure and streak evidence without granting execution authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_BACKTEST_ENGINE_H
#define UMICOM_STRATEGY_RESEARCH_BACKTEST_ENGINE_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStrategyBacktestState {
    double initialEquity;
    double equity;
    double peakEquity;
    double grossProfit;
    double grossLoss;
    double netProfit;
    double turnover;
    double commission;
    double slippage;
    double maxDrawdown;
    double totalWin;
    double totalLoss;
    uint64_t tradeCount;
    uint64_t winCount;
    uint64_t lossCount;
    uint64_t winStreak;
    uint64_t lossStreak;
    uint64_t maxWinStreak;
    uint64_t maxLossStreak;
    uint64_t exposureMilliseconds;
    uint64_t firstTradeMilliseconds;
    uint64_t lastTradeMilliseconds;
} UmiStrategyBacktestState;

typedef struct UmiStrategyBacktestSnapshot {
    double initialEquity;
    double endingEquity;
    double grossProfit;
    double grossLoss;
    double netProfit;
    double maxDrawdown;
    double maxDrawdownPercent;
    double profitFactor;
    double winRate;
    double averageWin;
    double averageLoss;
    double expectancy;
    double turnover;
    double commission;
    double slippage;
    double exposurePercent;
    uint64_t tradeCount;
    uint64_t winCount;
    uint64_t lossCount;
    uint64_t maxWinStreak;
    uint64_t maxLossStreak;
} UmiStrategyBacktestSnapshot;

UmiStatus umi_strategy_backtest_state_init(
    UmiStrategyBacktestState *state,
    double initialEquity);

UmiStatus umi_strategy_backtest_add_trade(
    UmiStrategyBacktestState *state,
    double entryPrice,
    double exitPrice,
    double signedQuantity,
    double commission,
    double slippage,
    uint64_t entryMilliseconds,
    uint64_t exitMilliseconds);

UmiStatus umi_strategy_backtest_snapshot(
    const UmiStrategyBacktestState *state,
    UmiStrategyBacktestSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
