/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_metric.c
 *
 * PURPOSE:
 *   Validate dashboard_metric analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_metric.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsDashboardMetric m;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_dashboard_metric_init(&m,"pnl","PnL","USD",12,10)!=0)return 1;return umi_analytics_dashboard_metric_change(&m)==2.0?0:2;}
