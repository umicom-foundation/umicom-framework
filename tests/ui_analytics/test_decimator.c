/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_decimator.c
 *
 * PURPOSE:
 *   Validate decimator analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/decimator.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){double v[]={1,9,3,4};UmiAnalyticsDecimatedPair p[2];/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_analytics_decimate_min_max(v,4,2,p,2)!=0)return 1;return p[0].minimum==1&&p[0].maximum==9?0:2;}
