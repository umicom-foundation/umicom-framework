/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_stacked_bar_chart.c
 *
 * PURPOSE:
 *   Validate stacked_bar_chart analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/stacked_bar_chart.h"

int main(void){double v[]={2,-1,3};UmiAnalyticsStackTotal t;if(umi_analytics_stacked_bar_total(v,3,&t)!=0)return 1;return t.positive==5.0&&t.negative==-1.0?0:2;}
