/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_service.c
 *
 * PURPOSE:
 *   Validate dashboard_service analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsDashboardService s;umi_analytics_dashboard_service_init(&s);/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_dashboard_service_register(&s,"trader")!=0)return 1;return umi_analytics_dashboard_service_contains(&s,"trader")?0:2;}
