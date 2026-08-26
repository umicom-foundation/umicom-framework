/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_context.c
 *
 * PURPOSE:
 *   Validate dashboard_context analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_context.h"

int main(void){UmiAnalyticsDashboardContext item;if(umi_analytics_dashboard_context_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_dashboard_context_valid(&item))?0:2;}
