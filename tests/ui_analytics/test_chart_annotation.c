/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_chart_annotation.c
 *
 * PURPOSE:
 *   Validate chart_annotation analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/chart_annotation.h"

int main(void){UmiAnalyticsChartAnnotation item;if(umi_analytics_chart_annotation_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_chart_annotation_valid(&item))?0:2;}
