/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_metric.c
 *
 * PURPOSE:
 *   Represent a dashboard metric with current, previous and unit semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_metric.h"

#include <string.h>
UmiStatus umi_analytics_dashboard_metric_init(UmiAnalyticsDashboardMetric *m,const char *id,const char *label,const char *unit,double value,double previous){UmiStatus s;if(m==NULL||!umi_analytics_number_valid(value)||!umi_analytics_number_valid(previous))return UMI_STATUS_INVALID_ARGUMENT;memset(m,0,sizeof *m);s=umi_analytics_copy_text(m->id,sizeof m->id,id);if(s!=0)return s;s=umi_analytics_copy_text(m->label,sizeof m->label,label);if(s!=0)return s;s=umi_analytics_copy_text(m->unit,sizeof m->unit,unit);if(s!=0)return s;m->value=value;m->previous=previous;return UMI_STATUS_OK;}
double umi_analytics_dashboard_metric_change(const UmiAnalyticsDashboardMetric *m){return m!=NULL?m->value-m->previous:0.0;}
