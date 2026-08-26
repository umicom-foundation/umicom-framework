/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_legend.c
 *
 * PURPOSE:
 *   Validate legend analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/legend.h"

int main(void){UmiAnalyticsLegend l;umi_analytics_legend_init(&l);if(umi_analytics_legend_add(&l,"p","Price")!=0)return 1;if(umi_analytics_legend_set_visible(&l,"p",0)!=0)return 2;return l.items[0].visible==0?0:3;}
