/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_kpi_model.c
 *
 * PURPOSE:
 *   Validate kpi_model analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/kpi_model.h"

int main(void){UmiAnalyticsKpiModel k;double p;if(umi_analytics_kpi_model_init(&k,0,100,50,1)!=0)return 1;if(umi_analytics_kpi_model_progress(&k,&p)!=0)return 2;return p==.5?0:3;}
