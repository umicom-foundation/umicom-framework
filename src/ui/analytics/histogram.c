/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/histogram.c
 *
 * PURPOSE:
 *   Build fixed-width histograms over finite numeric samples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/histogram.h"

#include <math.h>
#include <string.h>
/*
 * Provide the analytics histogram build operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_histogram_build(const double *values,size_t count,size_t bins,UmiAnalyticsHistogram *out_histogram){size_t i,seen=0U;double lo=0.0,hi=0.0,span;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values==NULL||out_histogram==NULL||bins==0U||bins>UMI_ANALYTICS_MAX_BINS)return UMI_STATUS_INVALID_ARGUMENT;memset(out_histogram,0,sizeof *out_histogram);/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(isfinite(values[i])){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(seen==0U)lo=hi=values[i];/* Use this fallback path when the earlier condition does not apply. */ else{/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]<lo)lo=values[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]>hi)hi=values[i];}seen++;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(seen==0U)return UMI_STATUS_NOT_FOUND;out_histogram->minimum=lo;out_histogram->maximum=hi;out_histogram->bin_count=bins;span=hi-lo;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(isfinite(values[i])){size_t idx;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(span==0.0)idx=0U;/* Use this fallback path when the earlier condition does not apply. */ else{double pos=(values[i]-lo)/span;idx=(size_t)floor(pos*(double)bins);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(idx>=bins)idx=bins-1U;}out_histogram->counts[idx]++;}return UMI_STATUS_OK;}
