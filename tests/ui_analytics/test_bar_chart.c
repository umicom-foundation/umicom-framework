/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_bar_chart.c
 *
 * PURPOSE:
 *   Validate bar_chart analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/bar_chart.h"

int main(void){UmiAnalyticsBarChart item;if(umi_analytics_bar_chart_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_bar_chart_valid(&item))?0:2;}
