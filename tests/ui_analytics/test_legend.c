/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_legend.c
 *
 * PURPOSE:
 *   Validate legend analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/legend.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsLegend l;umi_analytics_legend_init(&l);/* Apply this operation only while the related capability or state is available. */ if(umi_analytics_legend_add(&l,"p","Price")!=0)return 1;/* Apply this operation only while the related capability or state is available. */ if(umi_analytics_legend_set_visible(&l,"p",0)!=0)return 2;return l.items[0].visible==0?0:3;}
