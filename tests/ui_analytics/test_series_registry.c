/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_series_registry.c
 *
 * PURPOSE:
 *   Validate series_registry analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/series_registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsSeries s;UmiAnalyticsSeriesRegistry r;umi_analytics_series_registry_init(&r);/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_series_init(&s,"a","A")!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_series_registry_add(&r,&s)!=0)return 2;return umi_analytics_series_registry_find(&r,"a")==&s?0:3;}
