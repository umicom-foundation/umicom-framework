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

/* The original accumulator is retained for source review. It accepted NaN and
 * infinity, and committed counts before checking representable arithmetic.
 * The replacement below validates a candidate before publishing any change.
 * Public structures and legacy metric conventions are deliberately retained. */
#if 0
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
#endif

/* Framework owns this check so research hosts cannot disagree about whether
 * a rejected trade has changed their shared accumulator. */
static int BacktestStateValid(const UmiStrategyBacktestState *s)
{
    if (s == NULL) return 0;
    const double values[] = {s->initialEquity,s->equity,s->peakEquity,
        s->grossProfit,s->grossLoss,s->netProfit,s->turnover,s->commission,
        s->slippage,s->maxDrawdown,s->totalWin,s->totalLoss};
    for (size_t i=0; i<sizeof values/sizeof values[0]; ++i)
        if (!isfinite(values[i])) return 0;
    return s->initialEquity>0.0 && s->peakEquity>0.0 &&
        s->turnover>=0.0 && s->commission>=0.0 && s->slippage>=0.0 &&
        s->grossProfit>=0.0 && s->grossLoss<=0.0 && s->maxDrawdown>=0.0 &&
        s->totalWin>=0.0 && s->totalLoss<=0.0 &&
        s->winCount<=s->tradeCount && s->lossCount==s->tradeCount-s->winCount &&
        s->winStreak<=s->winCount && s->lossStreak<=s->lossCount;
}
UmiStatus umi_strategy_backtest_state_init(UmiStrategyBacktestState *state, double initialEquity)
{
    if (state==NULL || !isfinite(initialEquity) || initialEquity<=0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStrategyBacktestState value={0};
    value.initialEquity=initialEquity; value.equity=initialEquity;
    value.peakEquity=initialEquity; *state=value;
    return UMI_STATUS_OK;
}
UmiStatus umi_strategy_backtest_add_trade(UmiStrategyBacktestState *state,
    double entryPrice, double exitPrice, double signedQuantity,
    double commission, double slippage, uint64_t entryMilliseconds, uint64_t exitMilliseconds)
{
    if (!BacktestStateValid(state) || !isfinite(entryPrice) || !isfinite(exitPrice) ||
        !isfinite(signedQuantity) || !isfinite(commission) || !isfinite(slippage) ||
        entryPrice<=0.0 || exitPrice<=0.0 || signedQuantity==0.0 ||
        commission<0.0 || slippage<0.0 || exitMilliseconds<entryMilliseconds)
        return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t duration=exitMilliseconds-entryMilliseconds;
    if (state->tradeCount==UINT64_MAX || duration>UINT64_MAX-state->exposureMilliseconds)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    double costs=commission+slippage;
    double gross=(exitPrice-entryPrice)*signedQuantity;
    double pnl=gross-costs;
    double turnover=fabs(entryPrice*signedQuantity)+fabs(exitPrice*signedQuantity);
    if (!isfinite(costs) || !isfinite(gross) || !isfinite(pnl) || !isfinite(turnover))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStrategyBacktestState next=*state;
    next.tradeCount++; next.commission+=commission; next.slippage+=slippage;
    next.turnover+=turnover; next.exposureMilliseconds+=duration;
    if (next.tradeCount==1U) next.firstTradeMilliseconds=entryMilliseconds;
    next.lastTradeMilliseconds=exitMilliseconds;
    if (pnl>=0.0) {
        next.grossProfit+=pnl; next.totalWin+=pnl; next.winCount++;
        next.winStreak++; next.lossStreak=0;
        if(next.winStreak>next.maxWinStreak) next.maxWinStreak=next.winStreak;
    } else {
        next.grossLoss+=pnl; next.totalLoss+=pnl; next.lossCount++;
        next.lossStreak++; next.winStreak=0;
        if(next.lossStreak>next.maxLossStreak) next.maxLossStreak=next.lossStreak;
    }
    next.netProfit+=pnl; next.equity=next.initialEquity+next.netProfit;
    if(next.equity>next.peakEquity) next.peakEquity=next.equity;
    double drawdown=next.peakEquity-next.equity;
    if(!isfinite(drawdown)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(drawdown>next.maxDrawdown) next.maxDrawdown=drawdown;
    if(!BacktestStateValid(&next)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *state=next; return UMI_STATUS_OK;
}
UmiStatus umi_strategy_backtest_snapshot(const UmiStrategyBacktestState *state,
    UmiStrategyBacktestSnapshot *outSnapshot)
{
    if(outSnapshot==NULL || !BacktestStateValid(state)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStrategyBacktestSnapshot v={0};
    v.initialEquity=state->initialEquity; v.endingEquity=state->equity;
    v.grossProfit=state->grossProfit; v.grossLoss=state->grossLoss;
    v.netProfit=state->netProfit; v.maxDrawdown=state->maxDrawdown;
    v.maxDrawdownPercent=(state->maxDrawdown/state->peakEquity)*100.0;
    /* Legacy conventions: zero-profit trades count as wins; a no-loss profit
     * factor remains the existing 100 sentinel. New reports label it unavailable. */
    v.profitFactor=state->grossLoss<0.0 ? state->grossProfit/fabs(state->grossLoss)
        : (state->grossProfit>0.0 ? 100.0 : 0.0);
    v.winRate=state->tradeCount ? ((double)state->winCount/(double)state->tradeCount)*100.0 : 0.0;
    v.averageWin=state->winCount ? state->totalWin/(double)state->winCount : 0.0;
    v.averageLoss=state->lossCount ? state->totalLoss/(double)state->lossCount : 0.0;
    v.expectancy=state->tradeCount ? state->netProfit/(double)state->tradeCount : 0.0;
    v.turnover=state->turnover; v.commission=state->commission; v.slippage=state->slippage;
    uint64_t elapsed=state->lastTradeMilliseconds>=state->firstTradeMilliseconds
        ? state->lastTradeMilliseconds-state->firstTradeMilliseconds : 0;
    v.exposurePercent=elapsed ? ((double)state->exposureMilliseconds/(double)elapsed)*100.0 : 0.0;
    if(v.exposurePercent>100.0) v.exposurePercent=100.0;
    v.tradeCount=state->tradeCount; v.winCount=state->winCount; v.lossCount=state->lossCount;
    v.maxWinStreak=state->maxWinStreak; v.maxLossStreak=state->maxLossStreak;
    if(!isfinite(v.maxDrawdownPercent) || !isfinite(v.profitFactor) ||
       !isfinite(v.winRate) || !isfinite(v.averageWin) || !isfinite(v.averageLoss) ||
       !isfinite(v.expectancy) || !isfinite(v.exposurePercent))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *outSnapshot=v; return UMI_STATUS_OK;
}
