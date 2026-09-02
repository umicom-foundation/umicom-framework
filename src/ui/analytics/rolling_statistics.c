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
/*
 * Provide the analytics rolling mean operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_rolling_mean(const double *values,size_t count,size_t window,double *out_values,size_t capacity){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values==NULL||out_values==NULL||window==0U||capacity<count)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<count;i++){size_t start=(i+1U>window)?i+1U-window:0U,j,n=0U;double sum=0.0;/* Visit each bounded item once so every record receives the same rule. */ for(j=start;j<=i;j++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(isfinite(values[j])){sum+=values[j];n++;}out_values[i]=n?sum/(double)n:0.0;}return UMI_STATUS_OK;}
