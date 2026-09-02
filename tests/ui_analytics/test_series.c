/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_series.c
 *
 * PURPOSE:
 *   Validate series analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/series.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsSeries s;UmiAnalyticsDataPoint p;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_series_init(&s,"px","Price")!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_series_append(&s,1.0,10.0)!=0)return 2;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_series_at(&s,0,&p)!=0)return 3;return p.y==10.0?0:4;}
