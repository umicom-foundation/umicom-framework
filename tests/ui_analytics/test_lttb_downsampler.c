/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_lttb_downsampler.c
 *
 * PURPOSE:
 *   Validate lttb_downsampler analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/lttb_downsampler.h"

int main(void){UmiAnalyticsDataPoint in[8],out[4];size_t i,n;for(i=0;i<8;i++)umi_analytics_data_point_init(&in[i],(double)i,(i==3)?10.0:(double)i);if(umi_analytics_downsample_lttb(in,8,out,4,&n)!=0)return 1;return n==4&&out[0].x==0.0&&out[3].x==7.0?0:2;}
