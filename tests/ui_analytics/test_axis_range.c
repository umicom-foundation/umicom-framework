/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_axis_range.c
 *
 * PURPOSE:
 *   Validate axis_range analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis_range.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsAxisRange r,p;umi_analytics_axis_range_reset(&r);umi_analytics_axis_range_include(&r,10);umi_analytics_axis_range_include(&r,20);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_analytics_axis_range_padded(&r,0.1,&p)!=0)return 1;return p.minimum==9.0&&p.maximum==21.0?0:2;}
