/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_scatter_chart.c
 *
 * PURPOSE:
 *   Validate scatter_chart analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/scatter_chart.h"

int main(void){UmiAnalyticsScatterChart item;if(umi_analytics_scatter_chart_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_scatter_chart_valid(&item))?0:2;}
