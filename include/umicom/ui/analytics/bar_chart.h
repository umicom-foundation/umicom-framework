/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/bar_chart.h
 *
 * PURPOSE:
 *   Configure grouped bar-chart orientation and relative gap.
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
#ifndef UMICOM_UI_ANALYTICS_BAR_CHART_H
#define UMICOM_UI_ANALYTICS_BAR_CHART_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics bar chart data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsBarChart { UmiAnalyticsOrientation orientation; double gap_ratio; } UmiAnalyticsBarChart;
/**
 * Initialise analytics bar chart from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_bar_chart_init(UmiAnalyticsBarChart *item);
/**
 * Check that analytics bar chart satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_bar_chart_valid(const UmiAnalyticsBarChart *item);

#ifdef __cplusplus
}
#endif

#endif
