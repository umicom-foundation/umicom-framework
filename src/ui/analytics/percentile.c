/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/percentile.c
 *
 * PURPOSE:
 *   Calculate bounded interpolated percentiles without mutating caller data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/percentile.h"

#include <math.h>
#include <stdlib.h>
static int cmp_double(const void *a,const void *b){double x=*(const double*)a,y=*(const double*)b;return (x>y)-(x<y);}
UmiStatus umi_analytics_percentile(const double *values,size_t count,double percentile,double *out_value){double copy[UMI_ANALYTICS_MAX_POINTS],rank,frac;size_t i,n=0U,lower,upper;if(values==NULL||out_value==NULL||percentile<0.0||percentile>100.0||count>UMI_ANALYTICS_MAX_POINTS)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<count;i++)if(isfinite(values[i]))copy[n++]=values[i];if(n==0U)return UMI_STATUS_NOT_FOUND;qsort(copy,n,sizeof copy[0],cmp_double);rank=(percentile/100.0)*(double)(n-1U);lower=(size_t)floor(rank);upper=(size_t)ceil(rank);frac=rank-(double)lower;*out_value=copy[lower]+(copy[upper]-copy[lower])*frac;return UMI_STATUS_OK;}
