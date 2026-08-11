/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/indicator.h
 *
 * PURPOSE:
 *   Compute reusable moving-average indicators over chart series.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Chart data is independent of GTK4, JavaScript and TradingView. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_INDICATOR_H
#define UMICOM_CHART_INDICATOR_H
#include "umicom/chart/series.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_chart_indicator_sma(const UmiChartSeries *input,size_t period,UmiChartSeries *output);
UmiStatus umi_chart_indicator_ema(const UmiChartSeries *input,size_t period,UmiChartSeries *output);
#ifdef __cplusplus
}
#endif
#endif
