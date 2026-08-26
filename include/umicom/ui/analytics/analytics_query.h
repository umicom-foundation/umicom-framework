/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/analytics_query.h
 *
 * PURPOSE:
 *   Describe provider-neutral analytical queries for dashboard datasets.
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
#ifndef UMICOM_UI_ANALYTICS_ANALYTICS_QUERY_H
#define UMICOM_UI_ANALYTICS_ANALYTICS_QUERY_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsQuery { char dataset_id[UMI_ANALYTICS_ID_CAPACITY]; char metric_id[UMI_ANALYTICS_ID_CAPACITY]; char group_by[UMI_ANALYTICS_ID_CAPACITY]; int64_t start_ns; int64_t end_ns; size_t limit; } UmiAnalyticsQuery;
UmiStatus umi_analytics_query_init(UmiAnalyticsQuery *q,const char *dataset,const char *metric);
int umi_analytics_query_valid(const UmiAnalyticsQuery *q);

#ifdef __cplusplus
}
#endif

#endif
