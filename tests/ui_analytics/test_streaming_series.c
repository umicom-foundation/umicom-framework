/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_streaming_series.c
 *
 * PURPOSE:
 *   Validate streaming_series analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/streaming_series.h"

int main(void){UmiAnalyticsStreamingSeries s;UmiAnalyticsTimePoint p;umi_analytics_streaming_series_init(&s);if(umi_analytics_streaming_series_push(&s,1,10)!=0)return 1;if(umi_analytics_streaming_series_at(&s,0,&p)!=0)return 2;return p.value==10?0:3;}
