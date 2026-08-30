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
UmiStatus umi_analytics_statistics_compute(const double *values,size_t count,UmiAnalyticsStatistics *out_stats){size_t i,n=0U;double mean=0.0,m2=0.0,sum=0.0,lo=0.0,hi=0.0;if(values==NULL||out_stats==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(out_stats,0,sizeof *out_stats);for(i=0;i<count;i++){double v=values[i],delta;if(!isfinite(v))continue;if(n==0U){lo=hi=v;}else{if(v<lo)lo=v;if(v>hi)hi=v;}n++;sum+=v;delta=v-mean;mean+=delta/(double)n;m2+=delta*(v-mean);}if(n==0U)return UMI_STATUS_NOT_FOUND;out_stats->count=n;out_stats->minimum=lo;out_stats->maximum=hi;out_stats->sum=sum;out_stats->mean=mean;out_stats->variance=n>1U?m2/(double)(n-1U):0.0;out_stats->standard_deviation=sqrt(out_stats->variance);return UMI_STATUS_OK;}
