/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/rolling_statistics.c
 *
 * PURPOSE:
 *   Compute rolling means over bounded trailing windows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/rolling_statistics.h"

#include <math.h>
UmiStatus umi_analytics_rolling_mean(const double *values,size_t count,size_t window,double *out_values,size_t capacity){size_t i;if(values==NULL||out_values==NULL||window==0U||capacity<count)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<count;i++){size_t start=(i+1U>window)?i+1U-window:0U,j,n=0U;double sum=0.0;for(j=start;j<=i;j++)if(isfinite(values[j])){sum+=values[j];n++;}out_values[i]=n?sum/(double)n:0.0;}return UMI_STATUS_OK;}
