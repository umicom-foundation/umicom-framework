/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_axis_scale.c
 *
 * PURPOSE:
 *   Validate axis_scale analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis_scale.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsAxis a;double n;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_axis_init(&a,"x","X",UMI_ANALYTICS_SCALE_LINEAR,0,10)!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_axis_scale_normalize(&a,5,&n)!=0)return 2;return n==0.5?0:3;}
