/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/streaming_series.c
 *
 * PURPOSE:
 *   Maintain a bounded chronological ring buffer for live analytical samples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/streaming_series.h"

#include <string.h>
/*
 * Initialise analytics streaming series from caller-provided values so later operations
 * receive a known state.
 */
void umi_analytics_streaming_series_init(UmiAnalyticsStreamingSeries *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Provide the analytics streaming series push operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_streaming_series_push(UmiAnalyticsStreamingSeries *s,int64_t ts,double value){size_t idx;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>0U){UmiAnalyticsTimePoint last;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_analytics_streaming_series_at(s,s->count-1U,&last)==0&&ts<last.timestamp_ns)return UMI_STATUS_INVALID_ARGUMENT;}idx=(s->head+s->count)%256U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count==256U){s->head=(s->head+1U)%256U;idx=(s->head+s->count-1U)%256U;}/* Use this fallback path when the earlier condition does not apply. */ else s->count++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_analytics_time_point_init(&s->points[idx],ts,value)!=0)return UMI_STATUS_INVALID_ARGUMENT;s->revision++;return UMI_STATUS_OK;}
/*
 * Find analytics streaming series while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_analytics_streaming_series_at(const UmiAnalyticsStreamingSeries *s,size_t logical_index,UmiAnalyticsTimePoint *out_point){size_t idx;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out_point==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(logical_index>=s->count)return UMI_STATUS_NOT_FOUND;idx=(s->head+logical_index)%256U;*out_point=s->points[idx];return UMI_STATUS_OK;}
