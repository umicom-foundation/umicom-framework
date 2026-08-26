/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_service.c
 *
 * PURPOSE:
 *   Validate dashboard_service analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_service.h"

int main(void){UmiAnalyticsDashboardService s;umi_analytics_dashboard_service_init(&s);if(umi_analytics_dashboard_service_register(&s,"trader")!=0)return 1;return umi_analytics_dashboard_service_contains(&s,"trader")?0:2;}
