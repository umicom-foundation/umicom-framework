/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_axis.c
 *
 * PURPOSE:
 *   Validate axis analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis.h"

int main(void){UmiAnalyticsAxis a;if(umi_analytics_axis_init(&a,"y","Value",UMI_ANALYTICS_SCALE_LINEAR,-1,1)!=0)return 1;return umi_analytics_axis_valid(&a)?0:2;}
