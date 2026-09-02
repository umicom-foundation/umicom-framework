/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/statistics.c
 *
 * PURPOSE:
 *   Compute numerically stable descriptive statistics using Welford accumulation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/statistics.h"

#include <math.h>
#include <string.h>
/*
 * Provide the analytics statistics compute operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_statistics_compute(const double *values,size_t count,UmiAnalyticsStatistics *out_stats){size_t i,n=0U;double mean=0.0,m2=0.0,sum=0.0,lo=0.0,hi=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values==NULL||out_stats==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(out_stats,0,sizeof *out_stats);/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<count;i++){double v=values[i],delta;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!isfinite(v))continue;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n==0U){lo=hi=v;}/* Use this fallback path when the earlier condition does not apply. */ else{/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v<lo)lo=v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v>hi)hi=v;}n++;sum+=v;delta=v-mean;mean+=delta/(double)n;m2+=delta*(v-mean);}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n==0U)return UMI_STATUS_NOT_FOUND;out_stats->count=n;out_stats->minimum=lo;out_stats->maximum=hi;out_stats->sum=sum;out_stats->mean=mean;out_stats->variance=n>1U?m2/(double)(n-1U):0.0;out_stats->standard_deviation=sqrt(out_stats->variance);return UMI_STATUS_OK;}
