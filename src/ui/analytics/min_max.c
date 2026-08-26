/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/min_max.c
 *
 * PURPOSE:
 *   Compute finite minimum and maximum values from numeric samples.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/min_max.h"

#include <math.h>
UmiStatus umi_analytics_min_max_compute(const double *values,size_t count,UmiAnalyticsMinMax *out_result){size_t i,seen=0U;double lo=0.0,hi=0.0;if(values==NULL||out_result==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<count;i++){double v=values[i];if(!isfinite(v))continue;if(seen==0U){lo=hi=v;}else{if(v<lo)lo=v;if(v>hi)hi=v;}seen++;}if(seen==0U)return UMI_STATUS_NOT_FOUND;out_result->minimum=lo;out_result->maximum=hi;out_result->count=seen;return UMI_STATUS_OK;}
