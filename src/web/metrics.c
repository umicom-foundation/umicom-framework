/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/metrics.c
 *
 * PURPOSE:
 *   Implement HTTP counters used by observability views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/metrics.h"
#include <string.h>
void umi_web_metrics_init(UmiWebMetrics *metrics){if(metrics!=NULL)(void)memset(metrics,0,sizeof(*metrics));}
void umi_web_metrics_record(UmiWebMetrics *metrics,size_t bytes_in,int status,size_t bytes_out){if(metrics==NULL)return;++metrics->requests;metrics->bytes_in+=(uint64_t)bytes_in;metrics->bytes_out+=(uint64_t)bytes_out;if(status>=200&&status<300)++metrics->responses_2xx;else if(status>=400&&status<500)++metrics->responses_4xx;else if(status>=500)++metrics->responses_5xx;}
