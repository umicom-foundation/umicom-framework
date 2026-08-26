/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/dashboard_snapshot.h
 *
 * PURPOSE:
 *   Represent immutable dashboard render/data snapshot metadata.
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
#ifndef UMICOM_UI_ANALYTICS_DASHBOARD_SNAPSHOT_H
#define UMICOM_UI_ANALYTICS_DASHBOARD_SNAPSHOT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsDashboardSnapshot { char dashboard_id[UMI_ANALYTICS_ID_CAPACITY]; uint64_t revision; int64_t generated_at_ns; uint32_t metric_count; int healthy; } UmiAnalyticsDashboardSnapshot;
UmiStatus umi_analytics_dashboard_snapshot_init(UmiAnalyticsDashboardSnapshot *item);
int umi_analytics_dashboard_snapshot_valid(const UmiAnalyticsDashboardSnapshot *item);

#ifdef __cplusplus
}
#endif

#endif
