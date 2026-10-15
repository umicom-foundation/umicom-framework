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
#include "umicom/base/value_archive.h"
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
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_performance_io_telemetry_init(UmiPerformanceIoTelemetry *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_io_telemetry_validate(const UmiPerformanceIoTelemetry *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_io_telemetry_observe(UmiPerformanceIoTelemetry *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_io_telemetry_same_identity(const UmiPerformanceIoTelemetry *left, const UmiPerformanceIoTelemetry *right);
/* Domain-specific policy helper for io telemetry. */
double umi_performance_io_telemetry_ratio(double numerator, double denominator);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_io_telemetry_archive_encode(const UmiPerformanceIoTelemetry *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_io_telemetry_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceIoTelemetry *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_performance_io_telemetry_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPerformanceIoTelemetry *expected, UmiPerformanceIoTelemetry *value);


#ifdef __cplusplus
}
#endif
#endif
