/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/gauge_model.c
 *
 * PURPOSE:
 *   Compose canonical Design System gauge specifications with normalized value semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/gauge_model.h"

#include <string.h>
UmiStatus umi_analytics_gauge_model_init(UmiAnalyticsGaugeModel *g,const char *id,double minimum,double maximum,double value){UmiStatus s;if(g==NULL||minimum>=maximum||value<minimum||value>maximum)return UMI_STATUS_INVALID_ARGUMENT;memset(g,0,sizeof *g);s=umi_analytics_copy_text(g->id,sizeof g->id,id);if(s!=0)return s;g->spec.minimum=minimum;g->spec.maximum=maximum;g->spec.value=value;g->spec.warning_threshold=minimum+(maximum-minimum)*0.7;g->spec.danger_threshold=minimum+(maximum-minimum)*0.9;return UMI_STATUS_OK;}
UmiStatus umi_analytics_gauge_model_normalized(const UmiAnalyticsGaugeModel *g,double *out_value){if(g==NULL||out_value==NULL||g->spec.minimum>=g->spec.maximum)return UMI_STATUS_INVALID_ARGUMENT;*out_value=(g->spec.value-g->spec.minimum)/(g->spec.maximum-g->spec.minimum);return UMI_STATUS_OK;}
