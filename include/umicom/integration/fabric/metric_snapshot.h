/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/metric_snapshot.h
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


/**
 * Represent the fabric metric snapshot data shared with callers of this public contract.
 */
typedef struct UmiFabricMetricSnapshot { uint64_t messages; uint64_t errors; uint64_t retries; uint64_t total_latency_us; uint64_t max_latency_us; } UmiFabricMetricSnapshot;
/**
 * Initialise fabric metric snapshot from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_metric_snapshot_init(UmiFabricMetricSnapshot *snapshot);
/**
 * Provide the fabric metric snapshot record operation used by this module and its client
 * applications.
 */
void umi_fabric_metric_snapshot_record(UmiFabricMetricSnapshot *snapshot,bool error,bool retry,uint64_t latency_us);
/**
 * Provide the fabric metric snapshot average latency operation used by this module and its
 * client applications.
 */
double umi_fabric_metric_snapshot_average_latency(const UmiFabricMetricSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
