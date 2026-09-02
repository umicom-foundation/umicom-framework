/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_alert_state.c
 *
 * PURPOSE:
 *   Validate alert_state analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/alert_state.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsAlertState s;umi_analytics_alert_state_init(&s);umi_analytics_alert_state_apply(&s,1,100);umi_analytics_alert_state_acknowledge(&s);return s.active&&s.acknowledged&&s.occurrences==1?0:1;}
