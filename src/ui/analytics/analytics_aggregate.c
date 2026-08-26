/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_aggregate.c
 *
 * PURPOSE:
 *   Compute standard count/sum/min/max/average aggregations over finite samples.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_aggregate.h"

#include <math.h>
UmiStatus umi_analytics_aggregate_compute(UmiAnalyticsAggregateKind kind,const double *values,size_t count,double *out_value){size_t i,n=0U;double sum=0.0,lo=0.0,hi=0.0;if(values==NULL||out_value==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<count;i++)if(isfinite(values[i])){if(n==0U)lo=hi=values[i];else{if(values[i]<lo)lo=values[i];if(values[i]>hi)hi=values[i];}sum+=values[i];n++;}if(kind==UMI_ANALYTICS_AGGREGATE_COUNT){*out_value=(double)n;return UMI_STATUS_OK;}if(n==0U)return UMI_STATUS_NOT_FOUND;switch(kind){case UMI_ANALYTICS_AGGREGATE_SUM:*out_value=sum;break;case UMI_ANALYTICS_AGGREGATE_MIN:*out_value=lo;break;case UMI_ANALYTICS_AGGREGATE_MAX:*out_value=hi;break;case UMI_ANALYTICS_AGGREGATE_AVERAGE:*out_value=sum/(double)n;break;default:return UMI_STATUS_INVALID_ARGUMENT;}return UMI_STATUS_OK;}
