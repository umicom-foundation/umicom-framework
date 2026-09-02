/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_snapshot.c
 *
 * PURPOSE:
 *   Validate dashboard_snapshot analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_snapshot.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsDashboardSnapshot item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_dashboard_snapshot_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_dashboard_snapshot_valid(&item))?0:2;}
