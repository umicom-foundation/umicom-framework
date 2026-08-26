/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_alert_rule.c
 *
 * PURPOSE:
 *   Validate alert_rule analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/alert_rule.h"

int main(void){UmiAnalyticsAlertRule r;if(umi_analytics_alert_rule_init(&r,"cpu",UMI_ANALYTICS_COMPARE_GT,80,UMI_ANALYTICS_SEVERITY_WARNING)!=0)return 1;return umi_analytics_alert_rule_triggered(&r,90)?0:2;}
