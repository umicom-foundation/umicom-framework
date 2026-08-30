/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/network_telemetry.h
 *
 * PURPOSE:
 *   Represent network throughput and latency telemetry for network telemetry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_NETWORK_TELEMETRY_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_NETWORK_TELEMETRY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceNetworkTelemetry {
    uint32_t structure_size;
    uint32_t api_version;
    char id[UMI_PERFORMANCE_ID_CAPACITY];
    char subject_id[UMI_PERFORMANCE_ID_CAPACITY];
    UmiPerformanceState state;
    UmiPerformanceSeverity severity;
    uint64_t sequence;
    uint64_t timestamp_ns;
    double value;
    double auxiliary;
    uint64_t count;
    bool enabled;
} UmiPerformanceNetworkTelemetry;

/* Initialise a versioned network telemetry record with stable identities. */
UmiStatus umi_performance_network_telemetry_init(UmiPerformanceNetworkTelemetry *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_network_telemetry_validate(const UmiPerformanceNetworkTelemetry *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_network_telemetry_observe(UmiPerformanceNetworkTelemetry *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_network_telemetry_same_identity(const UmiPerformanceNetworkTelemetry *left, const UmiPerformanceNetworkTelemetry *right);
/* Domain-specific policy helper for network telemetry. */
double umi_performance_network_telemetry_ratio(double numerator, double denominator);

#ifdef __cplusplus
}
#endif
#endif
