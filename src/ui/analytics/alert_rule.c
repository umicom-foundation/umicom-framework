/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/alert_rule.c
 *
 * PURPOSE:
 *   Evaluate reusable threshold alert rules over dashboard and analytical metrics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/alert_rule.h"

#include <string.h>
UmiStatus umi_analytics_alert_rule_init(UmiAnalyticsAlertRule *r,const char *id,UmiAnalyticsCompare comparison,double threshold,UmiAnalyticsSeverity severity){UmiStatus s;if(r==NULL||comparison<1||comparison>6||severity<1||severity>4)return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof *r);s=umi_analytics_copy_text(r->id,sizeof r->id,id);if(s!=0)return s;r->comparison=comparison;r->threshold=threshold;r->severity=severity;return UMI_STATUS_OK;}
int umi_analytics_alert_rule_triggered(const UmiAnalyticsAlertRule *r,double value){return r!=NULL?umi_analytics_compare(value,r->comparison,r->threshold):0;}
