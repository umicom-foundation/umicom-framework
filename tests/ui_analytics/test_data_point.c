/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_data_point.c
 *
 * PURPOSE:
 *   Validate data_point analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/data_point.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsDataPoint p; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_data_point_init(&p,1.0,2.0)!=UMI_STATUS_OK)return 1; return p.y==2.0?0:2;}
