/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_chart_annotation.c
 *
 * PURPOSE:
 *   Validate chart_annotation analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/chart_annotation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsChartAnnotation item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_chart_annotation_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_chart_annotation_valid(&item))?0:2;}
