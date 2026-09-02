/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/alert_rule.c
 *
 * PURPOSE:
 *   Evaluate reusable threshold alert rules over dashboard and analytical metrics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/alert_rule.h"

#include <string.h>
/*
 * Initialise analytics alert rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_alert_rule_init(UmiAnalyticsAlertRule *r,const char *id,UmiAnalyticsCompare comparison,double threshold,UmiAnalyticsSeverity severity){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||comparison<1||comparison>6||severity<1||severity>4)return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof *r);s=umi_analytics_copy_text(r->id,sizeof r->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;r->comparison=comparison;r->threshold=threshold;r->severity=severity;return UMI_STATUS_OK;}
/*
 * Provide the analytics alert rule triggered operation used by this module and its client
 * applications.
 */
int umi_analytics_alert_rule_triggered(const UmiAnalyticsAlertRule *r,double value){return r!=NULL?umi_analytics_compare(value,r->comparison,r->threshold):0;}
