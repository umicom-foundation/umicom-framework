/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_data_window.c
 *
 * PURPOSE:
 *   Validate data_window analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/data_window.h"

int main(void){UmiAnalyticsDataWindow w;if(umi_analytics_data_window_resolve(100,90,20,&w)!=0)return 1;return w.count==10?0:2;}
