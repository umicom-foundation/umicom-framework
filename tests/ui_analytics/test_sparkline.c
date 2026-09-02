/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_sparkline.c
 *
 * PURPOSE:
 *   Validate sparkline analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/sparkline.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){double v[]={1,4,2};UmiAnalyticsSparklineSummary s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_analytics_sparkline_summarize(v,3,&s)!=0)return 1;return s.change==1.0&&s.maximum==4.0?0:2;}
