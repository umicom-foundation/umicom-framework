/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/metric_snapshot.c
 *
 * PURPOSE:
 *   Aggregate message, error and latency counters into a point-in-time Fabric metric snapshot.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/metric_snapshot.h"
#include <string.h>
#include <limits.h>


void umi_fabric_metric_snapshot_init(UmiFabricMetricSnapshot *snapshot){if(snapshot!=NULL)(void)memset(snapshot,0,sizeof(*snapshot));}
void umi_fabric_metric_snapshot_record(UmiFabricMetricSnapshot *s,bool error,bool retry,uint64_t latency_us){if(s==NULL)return;s->messages++;if(error)s->errors++;if(retry)s->retries++;if(UINT64_MAX-s->total_latency_us>=latency_us)s->total_latency_us+=latency_us;else s->total_latency_us=UINT64_MAX;if(latency_us>s->max_latency_us)s->max_latency_us=latency_us;}
double umi_fabric_metric_snapshot_average_latency(const UmiFabricMetricSnapshot *s){return s!=NULL&&s->messages>0U?(double)s->total_latency_us/(double)s->messages:0.0;}
