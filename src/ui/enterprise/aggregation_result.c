/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/aggregation_result.c
 *
 * PURPOSE:
 *   Implement incremental numeric aggregation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/aggregation_result.h"
void umi_ui_ent_aggregation_result_init(UmiUiEntAggregationResult *r,UmiUiEntAggregateKind k){if(r){r->kind=k;r->count=0U;r->sum=0.0;r->minimum=0.0;r->maximum=0.0;r->initialized=0;}}
UmiStatus umi_ui_ent_aggregation_result_add(UmiUiEntAggregationResult *r,double v){if(!r)return UMI_STATUS_INVALID_ARGUMENT;if(!r->initialized){r->minimum=v;r->maximum=v;r->initialized=1;}else{if(v<r->minimum)r->minimum=v;if(v>r->maximum)r->maximum=v;}r->sum+=v;r->count++;return UMI_STATUS_OK;}
double umi_ui_ent_aggregation_result_value(const UmiUiEntAggregationResult *r){if(!r||r->count==0U)return 0.0;switch(r->kind){case UMI_UI_ENT_AGG_COUNT:return (double)r->count;case UMI_UI_ENT_AGG_SUM:return r->sum;case UMI_UI_ENT_AGG_MIN:return r->minimum;case UMI_UI_ENT_AGG_MAX:return r->maximum;case UMI_UI_ENT_AGG_AVERAGE:return r->sum/(double)r->count;default:return 0.0;}}
