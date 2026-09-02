/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_min_max.c
 *
 * PURPOSE:
 *   Validate min_max analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/min_max.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){double v[]={3,1,9};UmiAnalyticsMinMax r;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_analytics_min_max_compute(v,3,&r)!=0)return 1;return r.minimum==1.0&&r.maximum==9.0?0:2;}
