/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/dashboard_service.h
 *
 * PURPOSE:
 *   Register Framework-owned dashboards for reuse across thin applications.
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
#ifndef UMICOM_UI_ANALYTICS_DASHBOARD_SERVICE_H
#define UMICOM_UI_ANALYTICS_DASHBOARD_SERVICE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsDashboardService { char ids[UMI_ANALYTICS_MAX_ITEMS][UMI_ANALYTICS_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAnalyticsDashboardService;
void umi_analytics_dashboard_service_init(UmiAnalyticsDashboardService *service);
UmiStatus umi_analytics_dashboard_service_register(UmiAnalyticsDashboardService *service,const char *id);
int umi_analytics_dashboard_service_contains(const UmiAnalyticsDashboardService *service,const char *id);

#ifdef __cplusplus
}
#endif

#endif
