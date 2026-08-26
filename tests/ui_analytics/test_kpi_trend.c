/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_kpi_trend.c
 *
 * PURPOSE:
 *   Validate kpi_trend analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/kpi_trend.h"

int main(void){double v[]={1,3,5};double s;if(umi_analytics_kpi_trend_slope(v,3,&s)!=0)return 1;return s==2.0?0:2;}
