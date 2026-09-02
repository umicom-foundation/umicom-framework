/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_linear_gauge.c
 *
 * PURPOSE:
 *   Validate linear_gauge analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/linear_gauge.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsLinearGauge item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_linear_gauge_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_linear_gauge_valid(&item))?0:2;}
