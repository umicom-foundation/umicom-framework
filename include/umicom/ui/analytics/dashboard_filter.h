/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/dashboard_filter.h
 *
 * PURPOSE:
 *   Describe one dashboard-level textual filter propagated to compatible tiles.
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
#ifndef UMICOM_UI_ANALYTICS_DASHBOARD_FILTER_H
#define UMICOM_UI_ANALYTICS_DASHBOARD_FILTER_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsDashboardFilter { char key[UMI_ANALYTICS_ID_CAPACITY]; char value[UMI_ANALYTICS_VALUE_CAPACITY]; int case_sensitive; } UmiAnalyticsDashboardFilter;
UmiStatus umi_analytics_dashboard_filter_init(UmiAnalyticsDashboardFilter *item);
int umi_analytics_dashboard_filter_valid(const UmiAnalyticsDashboardFilter *item);

#ifdef __cplusplus
}
#endif

#endif
