/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_chart_model.c
 *
 * PURPOSE:
 *   Validate chart_model analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/chart_model.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsChartModel c;UmiAnalyticsSeries s;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_chart_model_init(&c,"c","Chart",UMI_DESIGN_CHART_LINE)!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_series_init(&s,"s","Series")!=0)return 2;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_chart_model_add_series(&c,&s)!=0)return 3;return c.spec.series_count==1U?0:4;}
