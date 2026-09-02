/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_stacked_bar_chart.c
 *
 * PURPOSE:
 *   Validate stacked_bar_chart analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/stacked_bar_chart.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){double v[]={2,-1,3};UmiAnalyticsStackTotal t;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_stacked_bar_total(v,3,&t)!=0)return 1;return t.positive==5.0&&t.negative==-1.0?0:2;}
