/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_viewport.c
 *
 * PURPOSE:
 *   Validate viewport analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/viewport.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsViewport v;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_viewport_init(&v,0,10,0,10)!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_viewport_zoom(&v,2,5,5)!=0)return 2;return v.x_min==2.5&&v.x_max==7.5?0:3;}
