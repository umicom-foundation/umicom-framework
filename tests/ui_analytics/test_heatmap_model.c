/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_heatmap_model.c
 *
 * PURPOSE:
 *   Validate heatmap_model analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_model.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsHeatmapModel m;double v;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_heatmap_model_init(&m,"risk",2,2,-1,1)!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_heatmap_model_set(&m,1,1,.5)!=0)return 2;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_heatmap_model_get(&m,1,1,&v)!=0)return 3;return v==.5?0:4;}
