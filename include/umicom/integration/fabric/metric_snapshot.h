/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/metric_snapshot.h
 *
 * PURPOSE:
 *   Aggregate message, error and latency counters into a point-in-time Fabric metric snapshot.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_METRIC_SNAPSHOT_H
#define UMICOM_INTEGRATION_FABRIC_METRIC_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricMetricSnapshot { uint64_t messages; uint64_t errors; uint64_t retries; uint64_t total_latency_us; uint64_t max_latency_us; } UmiFabricMetricSnapshot;
void umi_fabric_metric_snapshot_init(UmiFabricMetricSnapshot *snapshot);
void umi_fabric_metric_snapshot_record(UmiFabricMetricSnapshot *snapshot,bool error,bool retry,uint64_t latency_us);
double umi_fabric_metric_snapshot_average_latency(const UmiFabricMetricSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
