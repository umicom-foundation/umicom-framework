/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_heatmap_model.c
 *
 * PURPOSE:
 *   Validate heatmap_model analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_model.h"

int main(void){UmiAnalyticsHeatmapModel m;double v;if(umi_analytics_heatmap_model_init(&m,"risk",2,2,-1,1)!=0)return 1;if(umi_analytics_heatmap_model_set(&m,1,1,.5)!=0)return 2;if(umi_analytics_heatmap_model_get(&m,1,1,&v)!=0)return 3;return v==.5?0:4;}
