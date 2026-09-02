/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_lttb_downsampler.c
 *
 * PURPOSE:
 *   Validate lttb_downsampler analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/lttb_downsampler.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsDataPoint in[8],out[4];size_t i,n;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<8;i++)umi_analytics_data_point_init(&in[i],(double)i,(i==3)?10.0:(double)i);/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_downsample_lttb(in,8,out,4,&n)!=0)return 1;return n==4&&out[0].x==0.0&&out[3].x==7.0?0:2;}
