/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_heatmap_scale.c
 *
 * PURPOSE:
 *   Validate heatmap_scale analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_scale.h"

int main(void){UmiAnalyticsHeatmapScale s;double n;if(umi_analytics_heatmap_scale_init(&s,-1,0,1,1)!=0)return 1;if(umi_analytics_heatmap_scale_normalize(&s,0,&n)!=0)return 2;return n==.5?0:3;}
