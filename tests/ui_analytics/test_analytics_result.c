/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_result.c
 *
 * PURPOSE:
 *   Validate analytics_result analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_result.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsResult r;umi_analytics_result_init(&r);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_analytics_result_add(&r,"A",5)!=0)return 1;return r.count==1&&r.rows[0].value==5?0:2;}
