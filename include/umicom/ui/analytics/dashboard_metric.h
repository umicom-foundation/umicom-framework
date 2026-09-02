/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/dashboard_metric.h
 *
 * PURPOSE:
 *   Represent a dashboard metric with current, previous and unit semantics.
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
#ifndef UMICOM_UI_ANALYTICS_DASHBOARD_METRIC_H
#define UMICOM_UI_ANALYTICS_DASHBOARD_METRIC_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics dashboard metric data shared with callers of this public
 * contract.
 */
typedef struct UmiAnalyticsDashboardMetric { char id[UMI_ANALYTICS_ID_CAPACITY]; char label[UMI_ANALYTICS_TEXT_CAPACITY]; char unit[64]; double value; double previous; } UmiAnalyticsDashboardMetric;
/**
 * Initialise analytics dashboard metric from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_metric_init(UmiAnalyticsDashboardMetric *m,const char *id,const char *label,const char *unit,double value,double previous);
/**
 * Provide the analytics dashboard metric change operation used by this module and its
 * client applications.
 */
double umi_analytics_dashboard_metric_change(const UmiAnalyticsDashboardMetric *m);

#ifdef __cplusplus
}
#endif

#endif
