/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_filter.c
 *
 * PURPOSE:
 *   Validate dashboard_filter analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_filter.h"

int main(void){UmiAnalyticsDashboardFilter item;if(umi_analytics_dashboard_filter_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_dashboard_filter_valid(&item))?0:2;}
