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
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance network telemetry data shared with callers of this public
 * contract.
 */
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_network_telemetry_archive_encode(const UmiPerformanceNetworkTelemetry *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_network_telemetry_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceNetworkTelemetry *value);

#ifdef __cplusplus
}
#endif
#endif
