/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_layout.c
 *
 * PURPOSE:
 *   Validate dashboard_layout analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_layout.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsDashboardLayout l;return umi_analytics_dashboard_layout_init(&l,"ops","Operations",6,12)==0&&l.grid.columns==12?0:1;}
