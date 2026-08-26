/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_query.c
 *
 * PURPOSE:
 *   Validate analytics_query analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_query.h"

int main(void){UmiAnalyticsQuery q;if(umi_analytics_query_init(&q,"market","price")!=0)return 1;return umi_analytics_query_valid(&q)?0:2;}
