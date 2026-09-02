/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_kpi_model.c
 *
 * PURPOSE:
 *   Validate kpi_model analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/kpi_model.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsKpiModel k;double p;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_kpi_model_init(&k,0,100,50,1)!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_kpi_model_progress(&k,&p)!=0)return 2;return p==.5?0:3;}
