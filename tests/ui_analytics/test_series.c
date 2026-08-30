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

int main(void){UmiAnalyticsSeries s;UmiAnalyticsDataPoint p;if(umi_analytics_series_init(&s,"px","Price")!=0)return 1;if(umi_analytics_series_append(&s,1.0,10.0)!=0)return 2;if(umi_analytics_series_at(&s,0,&p)!=0)return 3;return p.y==10.0?0:4;}
