/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_area_chart.c
 *
 * PURPOSE:
 *   Validate area_chart analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/area_chart.h"

int main(void){UmiAnalyticsAreaChart item;if(umi_analytics_area_chart_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_area_chart_valid(&item))?0:2;}
