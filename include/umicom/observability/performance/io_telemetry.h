/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/io_telemetry.h
 *
 * PURPOSE:
 *   Represent I/O throughput and latency telemetry for io telemetry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_IO_TELEMETRY_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_IO_TELEMETRY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance io telemetry data shared with callers of this public contract.
 */
typedef struct UmiPerformanceIoTelemetry {
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
} UmiPerformanceIoTelemetry;

/* Initialise a versioned io telemetry record with stable identities. */
UmiStatus umi_performance_io_telemetry_init(UmiPerformanceIoTelemetry *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_io_telemetry_validate(const UmiPerformanceIoTelemetry *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_io_telemetry_observe(UmiPerformanceIoTelemetry *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_io_telemetry_same_identity(const UmiPerformanceIoTelemetry *left, const UmiPerformanceIoTelemetry *right);
/* Domain-specific policy helper for io telemetry. */
double umi_performance_io_telemetry_ratio(double numerator, double denominator);

#ifdef __cplusplus
}
#endif
#endif
