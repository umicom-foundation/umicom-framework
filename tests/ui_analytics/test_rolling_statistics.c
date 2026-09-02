/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_rolling_statistics.c
 *
 * PURPOSE:
 *   Validate rolling_statistics analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/rolling_statistics.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){double v[]={1,3,5},o[3];/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_rolling_mean(v,3,2,o,3)!=0)return 1;return o[2]==4.0?0:2;}
