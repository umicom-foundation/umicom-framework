/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_percentile.c
 *
 * PURPOSE:
 *   Validate percentile analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/percentile.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){double v[]={0,10,20,30},p;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_percentile(v,4,50,&p)!=0)return 1;return p==15.0?0:2;}
