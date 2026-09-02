/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_tooltip.c
 *
 * PURPOSE:
 *   Validate tooltip analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/tooltip.h"

#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsTooltip t;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_analytics_tooltip_set(&t,"Point",1,2)!=0)return 1;return strstr(t.text,"y=2")!=NULL?0:2;}
