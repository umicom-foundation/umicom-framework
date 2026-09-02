/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_statistics.c
 *
 * PURPOSE:
 *   Validate statistics analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/statistics.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){double v[]={1,2,3};UmiAnalyticsStatistics s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_analytics_statistics_compute(v,3,&s)!=0)return 1;return s.mean==2.0&&s.count==3?0:2;}
