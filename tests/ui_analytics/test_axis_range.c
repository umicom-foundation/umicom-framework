/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_axis_range.c
 *
 * PURPOSE:
 *   Validate axis_range analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis_range.h"

int main(void){UmiAnalyticsAxisRange r,p;umi_analytics_axis_range_reset(&r);umi_analytics_axis_range_include(&r,10);umi_analytics_axis_range_include(&r,20);if(umi_analytics_axis_range_padded(&r,0.1,&p)!=0)return 1;return p.minimum==9.0&&p.maximum==21.0?0:2;}
