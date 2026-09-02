/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_axis.c
 *
 * PURPOSE:
 *   Validate axis analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsAxis a;/* Apply this operation only while the related capability or state is available. */ if(umi_analytics_axis_init(&a,"y","Value",UMI_ANALYTICS_SCALE_LINEAR,-1,1)!=0)return 1;return umi_analytics_axis_valid(&a)?0:2;}
