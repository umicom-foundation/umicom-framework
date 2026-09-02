/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_kpi_trend.c
 *
 * PURPOSE:
 *   Validate kpi_trend analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/kpi_trend.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){double v[]={1,3,5};double s;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_kpi_trend_slope(v,3,&s)!=0)return 1;return s==2.0?0:2;}
