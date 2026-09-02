/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/metric_snapshot.c
 *
 * PURPOSE:
 *   Aggregate message, error and latency counters into a point-in-time Fabric metric snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/metric_snapshot.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric metric snapshot from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_metric_snapshot_init(UmiFabricMetricSnapshot *snapshot){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(snapshot!=NULL)(void)memset(snapshot,0,sizeof(*snapshot));}
/*
 * Provide the fabric metric snapshot record operation used by this module and its client
 * applications.
 */
void umi_fabric_metric_snapshot_record(UmiFabricMetricSnapshot *s,bool error,bool retry,uint64_t latency_us){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;s->messages++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(error)s->errors++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(retry)s->retries++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(UINT64_MAX-s->total_latency_us>=latency_us)s->total_latency_us+=latency_us;/* Use this fallback path when the earlier condition does not apply. */ else s->total_latency_us=UINT64_MAX;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(latency_us>s->max_latency_us)s->max_latency_us=latency_us;}
/*
 * Provide the fabric metric snapshot average latency operation used by this module and its
 * client applications.
 */
double umi_fabric_metric_snapshot_average_latency(const UmiFabricMetricSnapshot *s){return s!=NULL&&s->messages>0U?(double)s->total_latency_us/(double)s->messages:0.0;}
