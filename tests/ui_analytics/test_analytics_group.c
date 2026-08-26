/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_group.c
 *
 * PURPOSE:
 *   Validate analytics_group analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_group.h"

int main(void){UmiAnalyticsGroup g;if(umi_analytics_group_init(&g,"Rates")!=0)return 1;umi_analytics_group_add(&g,2);umi_analytics_group_add(&g,3);return g.sum==5.0&&g.count==2?0:2;}
