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

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/metrics.h"
#include <string.h>
/*
 * Initialise web metrics from caller-provided values so later operations receive a known
 * state.
 */
void umi_web_metrics_init(UmiWebMetrics *metrics){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(metrics!=NULL)(void)memset(metrics,0,sizeof(*metrics));}
/*
 * Provide the web metrics record operation used by this module and its client
 * applications.
 */
void umi_web_metrics_record(UmiWebMetrics *metrics,size_t bytes_in,int status,size_t bytes_out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(metrics==NULL)return;++metrics->requests;metrics->bytes_in+=(uint64_t)bytes_in;metrics->bytes_out+=(uint64_t)bytes_out;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(status>=200&&status<300)++metrics->responses_2xx;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(status>=400&&status<500)++metrics->responses_4xx;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(status>=500)++metrics->responses_5xx;}
