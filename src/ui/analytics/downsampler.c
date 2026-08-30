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
UmiStatus umi_analytics_downsample_uniform(const UmiAnalyticsDataPoint *input,size_t input_count,UmiAnalyticsDataPoint *output,size_t target_count,size_t *out_count){size_t i;if(input==NULL||output==NULL||out_count==NULL||target_count==0U)return UMI_STATUS_INVALID_ARGUMENT;if(input_count<=target_count){for(i=0;i<input_count;i++)output[i]=input[i];*out_count=input_count;return UMI_STATUS_OK;}if(target_count==1U){output[0]=input[0];*out_count=1U;return UMI_STATUS_OK;}for(i=0;i<target_count;i++){double pos=(double)i*(double)(input_count-1U)/(double)(target_count-1U);size_t idx=(size_t)llround(pos);if(idx>=input_count)idx=input_count-1U;output[i]=input[idx];}*out_count=target_count;return UMI_STATUS_OK;}
