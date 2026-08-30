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

int main(void){UmiAnalyticsSeries s;UmiAnalyticsSeriesRegistry r;umi_analytics_series_registry_init(&r);if(umi_analytics_series_init(&s,"a","A")!=0)return 1;if(umi_analytics_series_registry_add(&r,&s)!=0)return 2;return umi_analytics_series_registry_find(&r,"a")==&s?0:3;}
