/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/decimator.c
 *
 * PURPOSE:
 *   Preserve bucket extrema when reducing dense analytical series.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/decimator.h"

#include <math.h>
UmiStatus umi_analytics_decimate_min_max(const double *values,size_t count,size_t buckets,UmiAnalyticsDecimatedPair *out_pairs,size_t capacity){size_t b;if(values==NULL||out_pairs==NULL||buckets==0U||capacity<buckets)return UMI_STATUS_INVALID_ARGUMENT;for(b=0;b<buckets;b++){size_t start=b*count/buckets,end=(b+1U)*count/buckets,i,seen=0U;double lo=0,hi=0;for(i=start;i<end;i++)if(isfinite(values[i])){if(seen==0U)lo=hi=values[i];else{if(values[i]<lo)lo=values[i];if(values[i]>hi)hi=values[i];}seen++;}if(seen==0U){lo=hi=0.0;}out_pairs[b].minimum=lo;out_pairs[b].maximum=hi;}return UMI_STATUS_OK;}
