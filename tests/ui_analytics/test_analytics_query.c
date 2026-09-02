/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_query.c
 *
 * PURPOSE:
 *   Validate analytics_query analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_query.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsQuery q;/* Apply this operation only while the related capability or state is available. */ if(umi_analytics_query_init(&q,"market","price")!=0)return 1;return umi_analytics_query_valid(&q)?0:2;}
