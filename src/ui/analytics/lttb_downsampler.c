/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/lttb_downsampler.c
 *
 * PURPOSE:
 *   Apply Largest-Triangle-Three-Buckets downsampling to preserve visual shape.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/lttb_downsampler.h"

#include <math.h>
UmiStatus umi_analytics_downsample_lttb(const UmiAnalyticsDataPoint *data,size_t n,UmiAnalyticsDataPoint *out,size_t threshold,size_t *out_count){size_t sampled=0U,a=0U,i;if(data==NULL||out==NULL||out_count==NULL||threshold<3U)return UMI_STATUS_INVALID_ARGUMENT;if(threshold>=n||n<=2U){for(i=0;i<n;i++)out[i]=data[i];*out_count=n;return UMI_STATUS_OK;}out[sampled++]=data[0];for(i=0;i<threshold-2U;i++){double every=(double)(n-2U)/(double)(threshold-2U);size_t range_start=(size_t)floor((double)i*every)+1U,range_end=(size_t)floor((double)(i+1U)*every)+1U,next_start=(size_t)floor((double)(i+1U)*every)+1U,next_end=(size_t)floor((double)(i+2U)*every)+1U,j,max_idx=range_start;double avg_x=0.0,avg_y=0.0,max_area=-1.0;if(range_end>=n)range_end=n-1U;if(next_end>n)next_end=n;if(next_start>=n)next_start=n-1U;{size_t cnt=next_end>next_start?next_end-next_start:1U;for(j=next_start;j<next_end;j++){avg_x+=data[j].x;avg_y+=data[j].y;}if(next_end<=next_start){avg_x=data[n-1U].x;avg_y=data[n-1U].y;}else{avg_x/=(double)cnt;avg_y/=(double)cnt;}}for(j=range_start;j<range_end;j++){double area=fabs((data[a].x-avg_x)*(data[j].y-data[a].y)-(data[a].x-data[j].x)*(avg_y-data[a].y));if(area>max_area){max_area=area;max_idx=j;}}out[sampled++]=data[max_idx];a=max_idx;}out[sampled++]=data[n-1U];*out_count=sampled;return UMI_STATUS_OK;}
