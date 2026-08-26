/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_viewport.c
 *
 * PURPOSE:
 *   Validate viewport analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/viewport.h"

int main(void){UmiAnalyticsViewport v;if(umi_analytics_viewport_init(&v,0,10,0,10)!=0)return 1;if(umi_analytics_viewport_zoom(&v,2,5,5)!=0)return 2;return v.x_min==2.5&&v.x_max==7.5?0:3;}
