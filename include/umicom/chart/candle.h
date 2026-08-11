/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/candle.h
 *
 * PURPOSE:
 *   Validate OHLCV candles and expose simple candle measurements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Chart data is independent of GTK4, JavaScript and TradingView. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_CANDLE_H
#define UMICOM_CHART_CANDLE_H
#include "umicom/chart/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_chart_candle_validate(const UmiChartCandle *candle);
double umi_chart_candle_range(const UmiChartCandle *candle);
int umi_chart_candle_is_up(const UmiChartCandle *candle);
#ifdef __cplusplus
}
#endif
#endif
