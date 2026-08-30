/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/kpi_model.c
 *
 * PURPOSE:
 *   Evaluate KPI progress against baseline and target values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/kpi_model.h"

UmiStatus umi_analytics_kpi_model_init(UmiAnalyticsKpiModel *k,double baseline,double target,double current,int higher_is_better){if(k==NULL||baseline==target)return UMI_STATUS_INVALID_ARGUMENT;k->baseline=baseline;k->target=target;k->current=current;k->higher_is_better=higher_is_better?1:0;return UMI_STATUS_OK;}
UmiStatus umi_analytics_kpi_model_progress(const UmiAnalyticsKpiModel *k,double *out_progress){double p;if(k==NULL||out_progress==NULL)return UMI_STATUS_INVALID_ARGUMENT;p=(k->current-k->baseline)/(k->target-k->baseline);if(!k->higher_is_better)p=1.0-p;*out_progress=p;return UMI_STATUS_OK;}
