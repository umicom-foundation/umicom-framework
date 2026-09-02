/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/aggregation_result.c
 *
 * PURPOSE:
 *   Implement incremental numeric aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/aggregation_result.h"
/*
 * Initialise ui ent aggregation result from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_aggregation_result_init(UmiUiEntAggregationResult *r,UmiUiEntAggregateKind k){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r){r->kind=k;r->count=0U;r->sum=0.0;r->minimum=0.0;r->maximum=0.0;r->initialized=0;}}
/*
 * Add ui ent aggregation result only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_aggregation_result_add(UmiUiEntAggregationResult *r,double v){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!r)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!r->initialized){r->minimum=v;r->maximum=v;r->initialized=1;}/* Use this fallback path when the earlier condition does not apply. */ else{/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v<r->minimum)r->minimum=v;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v>r->maximum)r->maximum=v;}r->sum+=v;r->count++;return UMI_STATUS_OK;}
/*
 * Provide the ui ent aggregation result value operation used by this module and its client
 * applications.
 */
double umi_ui_ent_aggregation_result_value(const UmiUiEntAggregationResult *r){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!r||r->count==0U)return 0.0;/* Select the behaviour associated with the requested command or state value. */ switch(r->kind){case UMI_UI_ENT_AGG_COUNT:return (double)r->count;case UMI_UI_ENT_AGG_SUM:return r->sum;case UMI_UI_ENT_AGG_MIN:return r->minimum;case UMI_UI_ENT_AGG_MAX:return r->maximum;case UMI_UI_ENT_AGG_AVERAGE:return r->sum/(double)r->count;default:return 0.0;}}
