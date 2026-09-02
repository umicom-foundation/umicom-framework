/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_data_window.c
 *
 * PURPOSE:
 *   Validate data_window analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/data_window.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsDataWindow w;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_analytics_data_window_resolve(100,90,20,&w)!=0)return 1;return w.count==10?0:2;}
