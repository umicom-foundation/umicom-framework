/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/min_max.c
 *
 * PURPOSE:
 *   Compute finite minimum and maximum values from numeric samples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/min_max.h"

#include <math.h>
/*
 * Provide the analytics min max compute operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_min_max_compute(const double *values,size_t count,UmiAnalyticsMinMax *out_result){size_t i,seen=0U;double lo=0.0,hi=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values==NULL||out_result==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<count;i++){double v=values[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!isfinite(v))continue;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(seen==0U){lo=hi=v;}/* Use this fallback path when the earlier condition does not apply. */ else{/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v<lo)lo=v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v>hi)hi=v;}seen++;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(seen==0U)return UMI_STATUS_NOT_FOUND;out_result->minimum=lo;out_result->maximum=hi;out_result->count=seen;return UMI_STATUS_OK;}
