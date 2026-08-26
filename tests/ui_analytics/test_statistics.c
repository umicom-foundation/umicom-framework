/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_statistics.c
 *
 * PURPOSE:
 *   Validate statistics analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/statistics.h"

int main(void){double v[]={1,2,3};UmiAnalyticsStatistics s;if(umi_analytics_statistics_compute(v,3,&s)!=0)return 1;return s.mean==2.0&&s.count==3?0:2;}
