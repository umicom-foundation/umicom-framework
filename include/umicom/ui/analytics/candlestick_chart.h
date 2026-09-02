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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_CANDLESTICK_CHART_H
#define UMICOM_UI_ANALYTICS_CANDLESTICK_CHART_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics candle data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsCandle { int64_t timestamp_ns; double open,high,low,close,volume; } UmiAnalyticsCandle;
/**
 * Initialise analytics candle from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_candle_init(UmiAnalyticsCandle *c,int64_t ts,double open,double high,double low,double close,double volume);
/**
 * Check that analytics candle satisfies its contract before another service relies on it.
 */
int umi_analytics_candle_valid(const UmiAnalyticsCandle *c);

#ifdef __cplusplus
}
#endif

#endif
