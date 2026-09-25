/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/backtest_engine.c
 *
 * PURPOSE:
 *   Implement deterministic backtest trade aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/backtest_engine.h"

#include <math.h>
#include <string.h>

UmiStatus umi_strategy_backtest_state_init(
    UmiStrategyBacktestState *state,
    double initialEquity)
{
    if (state == NULL || initialEquity <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(state, 0, sizeof(*state));
    state->initialEquity = initialEquity;
    state->equity = initialEquity;
    state->peakEquity = initialEquity;
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_backtest_add_trade(
    UmiStrategyBacktestState *state,
    double entryPrice,
    double exitPrice,
    double signedQuantity,
    double commission,
    double slippage,
    uint64_t entryMilliseconds,
    uint64_t exitMilliseconds)
{
    double pnl;
    double costs;
    double drawdown;

    if (state == NULL || entryPrice <= 0.0 || exitPrice <= 0.0 ||
        signedQuantity == 0.0 || commission < 0.0 || slippage < 0.0 ||
        exitMilliseconds < entryMilliseconds) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    costs = commission + slippage;
    pnl = (exitPrice - entryPrice) * signedQuantity - costs;

    state->tradeCount += 1U;
    state->commission += commission;
    state->slippage += slippage;
    state->turnover +=
        fabs(entryPrice * signedQuantity) +
        fabs(exitPrice * signedQuantity);
    state->exposureMilliseconds +=
        exitMilliseconds - entryMilliseconds;

    if (state->tradeCount == 1U) {
        state->firstTradeMilliseconds = entryMilliseconds;
    }
    state->lastTradeMilliseconds = exitMilliseconds;

    if (pnl >= 0.0) {
        state->grossProfit += pnl;
        state->totalWin += pnl;
        state->winCount += 1U;
        state->winStreak += 1U;
        state->lossStreak = 0U;
        if (state->winStreak > state->maxWinStreak) {
            state->maxWinStreak = state->winStreak;
        }
    } else {
        state->grossLoss += pnl;
        state->totalLoss += pnl;
        state->lossCount += 1U;
        state->lossStreak += 1U;
        state->winStreak = 0U;
        if (state->lossStreak > state->maxLossStreak) {
            state->maxLossStreak = state->lossStreak;
        }
    }

    state->netProfit += pnl;
    state->equity = state->initialEquity + state->netProfit;
    if (state->equity > state->peakEquity) {
        state->peakEquity = state->equity;
    }
    drawdown = state->peakEquity - state->equity;
    if (drawdown > state->maxDrawdown) {
        state->maxDrawdown = drawdown;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_backtest_snapshot(
    const UmiStrategyBacktestState *state,
    UmiStrategyBacktestSnapshot *outSnapshot)
{
    uint64_t completed;
    uint64_t elapsed;

    if (state == NULL || outSnapshot == NULL || state->initialEquity <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    outSnapshot->initialEquity = state->initialEquity;
    outSnapshot->endingEquity = state->equity;
    outSnapshot->grossProfit = state->grossProfit;
    outSnapshot->grossLoss = state->grossLoss;
    outSnapshot->netProfit = state->netProfit;
    outSnapshot->maxDrawdown = state->maxDrawdown;
    outSnapshot->maxDrawdownPercent =
        state->peakEquity > 0.0
            ? (state->maxDrawdown / state->peakEquity) * 100.0
            : 0.0;
    outSnapshot->profitFactor =
        state->grossLoss < 0.0
            ? state->grossProfit / fabs(state->grossLoss)
            : (state->grossProfit > 0.0 ? 100.0 : 0.0);

    completed = state->winCount + state->lossCount;
    outSnapshot->winRate =
        completed > 0U
            ? ((double)state->winCount / (double)completed) * 100.0
            : 0.0;
    outSnapshot->averageWin =
        state->winCount > 0U
            ? state->totalWin / (double)state->winCount
            : 0.0;
    outSnapshot->averageLoss =
        state->lossCount > 0U
            ? state->totalLoss / (double)state->lossCount
            : 0.0;
    outSnapshot->expectancy =
        completed > 0U
            ? state->netProfit / (double)completed
            : 0.0;
    outSnapshot->turnover = state->turnover;
    outSnapshot->commission = state->commission;
    outSnapshot->slippage = state->slippage;

    elapsed =
        state->lastTradeMilliseconds >= state->firstTradeMilliseconds
            ? state->lastTradeMilliseconds - state->firstTradeMilliseconds
            : 0U;
    outSnapshot->exposurePercent =
        elapsed > 0U
            ? ((double)state->exposureMilliseconds /
               (double)elapsed) * 100.0
            : 0.0;
    if (outSnapshot->exposurePercent > 100.0) {
        outSnapshot->exposurePercent = 100.0;
    }

    outSnapshot->tradeCount = state->tradeCount;
    outSnapshot->winCount = state->winCount;
    outSnapshot->lossCount = state->lossCount;
    outSnapshot->maxWinStreak = state->maxWinStreak;
    outSnapshot->maxLossStreak = state->maxLossStreak;
    return UMI_STATUS_OK;
}
