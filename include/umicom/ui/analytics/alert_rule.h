/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/alert_rule.h
 *
 * PURPOSE:
 *   Evaluate reusable threshold alert rules over dashboard and analytical metrics.
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
#ifndef UMICOM_UI_ANALYTICS_ALERT_RULE_H
#define UMICOM_UI_ANALYTICS_ALERT_RULE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsAlertRule { char id[UMI_ANALYTICS_ID_CAPACITY]; UmiAnalyticsCompare comparison; double threshold; UmiAnalyticsSeverity severity; } UmiAnalyticsAlertRule;
UmiStatus umi_analytics_alert_rule_init(UmiAnalyticsAlertRule *r,const char *id,UmiAnalyticsCompare comparison,double threshold,UmiAnalyticsSeverity severity);
int umi_analytics_alert_rule_triggered(const UmiAnalyticsAlertRule *r,double value);

#ifdef __cplusplus
}
#endif

#endif
