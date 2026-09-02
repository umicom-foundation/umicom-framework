/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_alert_rule.c
 *
 * PURPOSE:
 *   Validate alert_rule analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/alert_rule.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsAlertRule r;/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_alert_rule_init(&r,"cpu",UMI_ANALYTICS_COMPARE_GT,80,UMI_ANALYTICS_SEVERITY_WARNING)!=0)return 1;return umi_analytics_alert_rule_triggered(&r,90)?0:2;}
