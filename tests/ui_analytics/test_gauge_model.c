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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsGaugeModel g;double n;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_gauge_model_init(&g,"cpu",0,100,25)!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_gauge_model_normalized(&g,&n)!=0)return 2;return n==0.25?0:3;}
