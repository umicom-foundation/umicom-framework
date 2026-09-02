/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/candle.h
 *
 * PURPOSE:
 *   Validate OHLCV candles and expose simple candle measurements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Chart data is independent of any frontend toolkit or renderer. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_CANDLE_H
#define UMICOM_CHART_CANDLE_H
#include "umicom/chart/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Check that chart candle satisfies its contract before another service relies on it.
 */
UmiStatus umi_chart_candle_validate(const UmiChartCandle *candle);
/**
 * Provide the chart candle range operation used by this module and its client
 * applications.
 */
double umi_chart_candle_range(const UmiChartCandle *candle);
/**
 * Provide the chart candle is up operation used by this module and its client
 * applications.
 */
int umi_chart_candle_is_up(const UmiChartCandle *candle);
#ifdef __cplusplus
}
#endif
#endif
