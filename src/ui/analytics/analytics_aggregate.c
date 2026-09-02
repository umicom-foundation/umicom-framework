/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_aggregate.c
 *
 * PURPOSE:
 *   Compute standard count/sum/min/max/average aggregations over finite samples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_aggregate.h"

#include <math.h>
/*
 * Provide the analytics aggregate compute operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_aggregate_compute(UmiAnalyticsAggregateKind kind,const double *values,size_t count,double *out_value){size_t i,n=0U;double sum=0.0,lo=0.0,hi=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values==NULL||out_value==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(isfinite(values[i])){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n==0U)lo=hi=values[i];/* Use this fallback path when the earlier condition does not apply. */ else{/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]<lo)lo=values[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]>hi)hi=values[i];}sum+=values[i];n++;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(kind==UMI_ANALYTICS_AGGREGATE_COUNT){*out_value=(double)n;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n==0U)return UMI_STATUS_NOT_FOUND;/* Select the behaviour associated with the requested command or state value. */ switch(kind){case UMI_ANALYTICS_AGGREGATE_SUM:*out_value=sum;break;case UMI_ANALYTICS_AGGREGATE_MIN:*out_value=lo;break;case UMI_ANALYTICS_AGGREGATE_MAX:*out_value=hi;break;case UMI_ANALYTICS_AGGREGATE_AVERAGE:*out_value=sum/(double)n;break;default:return UMI_STATUS_INVALID_ARGUMENT;}return UMI_STATUS_OK;}
