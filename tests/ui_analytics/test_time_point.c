/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_time_point.c
 *
 * PURPOSE:
 *   Validate time_point analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/time_point.h"

int main(void){UmiAnalyticsTimePoint a,b; if(umi_analytics_time_point_init(&a,1,2.0)!=0)return 1; if(umi_analytics_time_point_init(&b,2,3.0)!=0)return 2; return umi_analytics_time_point_compare(&a,&b)<0?0:3;}
