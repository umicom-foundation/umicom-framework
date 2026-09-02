/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_correlation_heatmap.c
 *
 * PURPOSE:
 *   Validate correlation_heatmap analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/correlation_heatmap.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsCorrelationHeatmap m;double v;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_correlation_heatmap_init(&m,3)!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_correlation_heatmap_set(&m,0,2,-.7)!=0)return 2;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_correlation_heatmap_get(&m,2,0,&v)!=0)return 3;return v==-.7?0:4;}
