/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/line_chart.h
 *
 * PURPOSE:
 *   Configure line-chart interpolation and marker semantics.
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
#ifndef UMICOM_UI_ANALYTICS_LINE_CHART_H
#define UMICOM_UI_ANALYTICS_LINE_CHART_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics line chart data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsLineChart { int smooth; int markers; double stroke_width; } UmiAnalyticsLineChart;
/**
 * Initialise analytics line chart from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_line_chart_init(UmiAnalyticsLineChart *item);
/**
 * Check that analytics line chart satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_line_chart_valid(const UmiAnalyticsLineChart *item);

#ifdef __cplusplus
}
#endif

#endif
