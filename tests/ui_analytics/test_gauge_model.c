/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_gauge_model.c
 *
 * PURPOSE:
 *   Validate gauge_model analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/gauge_model.h"

int main(void){UmiAnalyticsGaugeModel g;double n;if(umi_analytics_gauge_model_init(&g,"cpu",0,100,25)!=0)return 1;if(umi_analytics_gauge_model_normalized(&g,&n)!=0)return 2;return n==0.25?0:3;}
