/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/dashboard_context.h
 *
 * PURPOSE:
 *   Carry linked entity and time-window context across dashboard tiles.
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
#ifndef UMICOM_UI_ANALYTICS_DASHBOARD_CONTEXT_H
#define UMICOM_UI_ANALYTICS_DASHBOARD_CONTEXT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsDashboardContext { char context_group[UMI_ANALYTICS_ID_CAPACITY]; char entity_id[UMI_ANALYTICS_ID_CAPACITY]; int64_t start_ns; int64_t end_ns; } UmiAnalyticsDashboardContext;
UmiStatus umi_analytics_dashboard_context_init(UmiAnalyticsDashboardContext *item);
int umi_analytics_dashboard_context_valid(const UmiAnalyticsDashboardContext *item);

#ifdef __cplusplus
}
#endif

#endif
