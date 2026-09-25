/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_backtest_engine.c
 *
 * PURPOSE:
 *   Verify deterministic P&L, cost, streak, drawdown and exposure aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>

#include "umicom/strategy_research/backtest_engine.h"

int main(void)
{
    UmiStrategyBacktestState state;
    UmiStrategyBacktestSnapshot snapshot;

    assert(umi_strategy_backtest_state_init(&state, 10000.0) == UMI_STATUS_OK);
    assert(umi_strategy_backtest_add_trade(
               &state, 100.0, 110.0, 10.0, 2.0, 1.0, 1000U, 2000U) ==
           UMI_STATUS_OK);
    assert(umi_strategy_backtest_add_trade(
               &state, 110.0, 105.0, 10.0, 2.0, 1.0, 2100U, 3100U) ==
           UMI_STATUS_OK);
    assert(umi_strategy_backtest_snapshot(&state, &snapshot) == UMI_STATUS_OK);

    assert(snapshot.tradeCount == 2U);
    assert(snapshot.winCount == 1U);
    assert(snapshot.lossCount == 1U);
    assert(fabs(snapshot.grossProfit - 97.0) < 0.001);
    assert(fabs(snapshot.grossLoss + 53.0) < 0.001);
    assert(fabs(snapshot.netProfit - 44.0) < 0.001);
    assert(snapshot.maxDrawdown > 52.9 && snapshot.maxDrawdown < 53.1);
    assert(snapshot.winRate > 49.9 && snapshot.winRate < 50.1);
    assert(snapshot.profitFactor > 1.82 && snapshot.profitFactor < 1.84);
    return 0;
}
