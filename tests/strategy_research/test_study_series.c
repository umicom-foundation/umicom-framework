/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_study_series.c
 *
 * PURPOSE:
 *   Verify deterministic SMA, EMA, RSI, ATR and VWAP study calculations.
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

#include "umicom/strategy_research/study_series.h"

int main(void)
{
    const double close[] = {100.0, 101.0, 102.0, 103.0, 104.0};
    const double high[] = {101.0, 102.0, 103.0, 104.0, 105.0};
    const double low[] = {99.0, 100.0, 101.0, 102.0, 103.0};
    const double volume[] = {10.0, 20.0, 30.0, 40.0, 50.0};
    double value;

    assert(umi_strategy_study_sma(close, 5U, 3U, &value) == UMI_STATUS_OK);
    assert(fabs(value - 103.0) < 0.001);

    assert(umi_strategy_study_ema(close, 5U, 3U, &value) == UMI_STATUS_OK);
    assert(value > 103.0 && value < 104.1);

    assert(umi_strategy_study_rsi(close, 5U, 4U, &value) == UMI_STATUS_OK);
    assert(fabs(value - 100.0) < 0.001);

    assert(umi_strategy_study_atr(high, low, close, 5U, 3U, &value) ==
           UMI_STATUS_OK);
    assert(fabs(value - 2.0) < 0.001);

    assert(umi_strategy_study_vwap(close, volume, 5U, &value) == UMI_STATUS_OK);
    assert(value > 102.6 && value < 102.8);
    return 0;
}
