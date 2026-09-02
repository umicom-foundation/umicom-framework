/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/decimator.c
 *
 * PURPOSE:
 *   Preserve bucket extrema when reducing dense analytical series.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/decimator.h"

#include <math.h>
/*
 * Provide the analytics decimate min max operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_decimate_min_max(const double *values,size_t count,size_t buckets,UmiAnalyticsDecimatedPair *out_pairs,size_t capacity){size_t b;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values==NULL||out_pairs==NULL||buckets==0U||capacity<buckets)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(b=0;b<buckets;b++){size_t start=b*count/buckets,end=(b+1U)*count/buckets,i,seen=0U;double lo=0,hi=0;/* Visit each bounded item once so every record receives the same rule. */ for(i=start;i<end;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(isfinite(values[i])){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(seen==0U)lo=hi=values[i];/* Use this fallback path when the earlier condition does not apply. */ else{/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]<lo)lo=values[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]>hi)hi=values[i];}seen++;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(seen==0U){lo=hi=0.0;}out_pairs[b].minimum=lo;out_pairs[b].maximum=hi;}return UMI_STATUS_OK;}
