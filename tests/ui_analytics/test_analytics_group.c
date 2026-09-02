/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_group.c
 *
 * PURPOSE:
 *   Validate analytics_group analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_group.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsGroup g;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_analytics_group_init(&g,"Rates")!=0)return 1;umi_analytics_group_add(&g,2);umi_analytics_group_add(&g,3);return g.sum==5.0&&g.count==2?0:2;}
