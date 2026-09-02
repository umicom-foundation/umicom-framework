/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/downsampler.c
 *
 * PURPOSE:
 *   Uniformly downsample ordered data while preserving endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/downsampler.h"

#include <math.h>
/*
 * Provide the analytics downsample uniform operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_downsample_uniform(const UmiAnalyticsDataPoint *input,size_t input_count,UmiAnalyticsDataPoint *output,size_t target_count,size_t *out_count){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(input==NULL||output==NULL||out_count==NULL||target_count==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(input_count<=target_count){/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<input_count;i++)output[i]=input[i];*out_count=input_count;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(target_count==1U){output[0]=input[0];*out_count=1U;return UMI_STATUS_OK;}/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<target_count;i++){double pos=(double)i*(double)(input_count-1U)/(double)(target_count-1U);size_t idx=(size_t)llround(pos);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(idx>=input_count)idx=input_count-1U;output[i]=input[idx];}*out_count=target_count;return UMI_STATUS_OK;}
