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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_BAR_CHART_H
#define UMICOM_UI_ANALYTICS_BAR_CHART_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsBarChart { UmiAnalyticsOrientation orientation; double gap_ratio; } UmiAnalyticsBarChart;
UmiStatus umi_analytics_bar_chart_init(UmiAnalyticsBarChart *item);
int umi_analytics_bar_chart_valid(const UmiAnalyticsBarChart *item);

#ifdef __cplusplus
}
#endif

#endif
