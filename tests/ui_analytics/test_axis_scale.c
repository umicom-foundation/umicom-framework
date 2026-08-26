/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_axis_scale.c
 *
 * PURPOSE:
 *   Validate axis_scale analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis_scale.h"

int main(void){UmiAnalyticsAxis a;double n;if(umi_analytics_axis_init(&a,"x","X",UMI_ANALYTICS_SCALE_LINEAR,0,10)!=0)return 1;if(umi_analytics_axis_scale_normalize(&a,5,&n)!=0)return 2;return n==0.5?0:3;}
