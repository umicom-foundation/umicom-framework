/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_downsampler.c
 *
 * PURPOSE:
 *   Validate downsampler analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/downsampler.h"

int main(void){UmiAnalyticsDataPoint in[5],out[3];size_t n,i;for(i=0;i<5;i++)umi_analytics_data_point_init(&in[i],(double)i,(double)i);if(umi_analytics_downsample_uniform(in,5,out,3,&n)!=0)return 1;return n==3&&out[2].x==4.0?0:2;}
