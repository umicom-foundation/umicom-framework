/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_heatmap_scale.c
 *
 * PURPOSE:
 *   Validate heatmap_scale analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_scale.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsHeatmapScale s;double n;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_heatmap_scale_init(&s,-1,0,1,1)!=0)return 1;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_heatmap_scale_normalize(&s,0,&n)!=0)return 2;return n==.5?0:3;}
