/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/scatter_chart.h
 *
 * PURPOSE:
 *   Configure scatter point radius and optional trend presentation.
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
#ifndef UMICOM_UI_ANALYTICS_SCATTER_CHART_H
#define UMICOM_UI_ANALYTICS_SCATTER_CHART_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics scatter chart data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsScatterChart { double point_radius; int trendline; } UmiAnalyticsScatterChart;
/**
 * Initialise analytics scatter chart from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_scatter_chart_init(UmiAnalyticsScatterChart *item);
/**
 * Check that analytics scatter chart satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_scatter_chart_valid(const UmiAnalyticsScatterChart *item);

#ifdef __cplusplus
}
#endif

#endif
