/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_metric.c
 *
 * PURPOSE:
 *   Represent a dashboard metric with current, previous and unit semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_metric.h"

#include <string.h>
/*
 * Initialise analytics dashboard metric from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_metric_init(UmiAnalyticsDashboardMetric *m,const char *id,const char *label,const char *unit,double value,double previous){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||!umi_analytics_number_valid(value)||!umi_analytics_number_valid(previous))return UMI_STATUS_INVALID_ARGUMENT;memset(m,0,sizeof *m);s=umi_analytics_copy_text(m->id,sizeof m->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;s=umi_analytics_copy_text(m->label,sizeof m->label,label);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;s=umi_analytics_copy_text(m->unit,sizeof m->unit,unit);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;m->value=value;m->previous=previous;return UMI_STATUS_OK;}
/*
 * Provide the analytics dashboard metric change operation used by this module and its
 * client applications.
 */
double umi_analytics_dashboard_metric_change(const UmiAnalyticsDashboardMetric *m){return m!=NULL?m->value-m->previous:0.0;}
