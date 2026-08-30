/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/area_chart.h
 *
 * PURPOSE:
 *   Configure area-chart stacking and fill opacity.
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
#ifndef UMICOM_UI_ANALYTICS_AREA_CHART_H
#define UMICOM_UI_ANALYTICS_AREA_CHART_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsAreaChart { int stacked; double opacity; } UmiAnalyticsAreaChart;
UmiStatus umi_analytics_area_chart_init(UmiAnalyticsAreaChart *item);
int umi_analytics_area_chart_valid(const UmiAnalyticsAreaChart *item);

#ifdef __cplusplus
}
#endif

#endif
