/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_result.c
 *
 * PURPOSE:
 *   Validate analytics_result analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_result.h"

int main(void){UmiAnalyticsResult r;umi_analytics_result_init(&r);if(umi_analytics_result_add(&r,"A",5)!=0)return 1;return r.count==1&&r.rows[0].value==5?0:2;}
