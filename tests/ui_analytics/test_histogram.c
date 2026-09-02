/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_histogram.c
 *
 * PURPOSE:
 *   Validate histogram analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/histogram.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){double v[]={0,1,2,3};UmiAnalyticsHistogram h;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_histogram_build(v,4,2,&h)!=0)return 1;return h.counts[0]==2&&h.counts[1]==2?0:2;}
