/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/time_bucket.c
 *
 * PURPOSE:
 *   Bucket nanosecond timestamps into deterministic fixed intervals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/time_bucket.h"

UmiStatus umi_analytics_time_bucket_floor(int64_t timestamp_ns,int64_t interval_ns,int64_t *out_bucket_start){int64_t q,r;if(out_bucket_start==NULL||interval_ns<=0)return UMI_STATUS_INVALID_ARGUMENT;q=timestamp_ns/interval_ns;r=timestamp_ns%interval_ns;if(r<0)q-=1;*out_bucket_start=q*interval_ns;return UMI_STATUS_OK;}
