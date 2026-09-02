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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsDataPoint in[5],out[3];size_t n,i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<5;i++)umi_analytics_data_point_init(&in[i],(double)i,(double)i);/* Apply this branch only when its contract condition is satisfied. */ if(umi_analytics_downsample_uniform(in,5,out,3,&n)!=0)return 1;return n==3&&out[2].x==4.0?0:2;}
