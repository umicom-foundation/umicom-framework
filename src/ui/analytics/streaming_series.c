/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/streaming_series.c
 *
 * PURPOSE:
 *   Maintain a bounded chronological ring buffer for live analytical samples.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/streaming_series.h"

#include <string.h>
void umi_analytics_streaming_series_init(UmiAnalyticsStreamingSeries *s){if(s!=NULL)memset(s,0,sizeof *s);}
UmiStatus umi_analytics_streaming_series_push(UmiAnalyticsStreamingSeries *s,int64_t ts,double value){size_t idx;if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(s->count>0U){UmiAnalyticsTimePoint last;if(umi_analytics_streaming_series_at(s,s->count-1U,&last)==0&&ts<last.timestamp_ns)return UMI_STATUS_INVALID_ARGUMENT;}idx=(s->head+s->count)%256U;if(s->count==256U){s->head=(s->head+1U)%256U;idx=(s->head+s->count-1U)%256U;}else s->count++;if(umi_analytics_time_point_init(&s->points[idx],ts,value)!=0)return UMI_STATUS_INVALID_ARGUMENT;s->revision++;return UMI_STATUS_OK;}
UmiStatus umi_analytics_streaming_series_at(const UmiAnalyticsStreamingSeries *s,size_t logical_index,UmiAnalyticsTimePoint *out_point){size_t idx;if(s==NULL||out_point==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(logical_index>=s->count)return UMI_STATUS_NOT_FOUND;idx=(s->head+logical_index)%256U;*out_point=s->points[idx];return UMI_STATUS_OK;}
