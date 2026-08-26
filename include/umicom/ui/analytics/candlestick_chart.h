/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/candlestick_chart.h
 *
 * PURPOSE:
 *   Validate OHLC analytical samples for financial and operational range charts.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_CANDLESTICK_CHART_H
#define UMICOM_UI_ANALYTICS_CANDLESTICK_CHART_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsCandle { int64_t timestamp_ns; double open,high,low,close,volume; } UmiAnalyticsCandle;
UmiStatus umi_analytics_candle_init(UmiAnalyticsCandle *c,int64_t ts,double open,double high,double low,double close,double volume);
int umi_analytics_candle_valid(const UmiAnalyticsCandle *c);

#ifdef __cplusplus
}
#endif

#endif
