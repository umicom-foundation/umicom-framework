/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_time_point.c
 *
 * PURPOSE:
 *   Validate time_point analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/time_point.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsTimePoint a,b; /* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_time_point_init(&a,1,2.0)!=0)return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_time_point_init(&b,2,3.0)!=0)return 2; return umi_analytics_time_point_compare(&a,&b)<0?0:3;}
