/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_configuration.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_CONFIGURATION_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_CONFIGURATION_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance profiler configuration data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceProfilerConfiguration {
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
} UmiPerformanceProfilerConfiguration;

/* Initialise a versioned profiler configuration record with stable identities. */
UmiStatus umi_performance_profiler_configuration_init(UmiPerformanceProfilerConfiguration *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_configuration_validate(const UmiPerformanceProfilerConfiguration *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_configuration_observe(UmiPerformanceProfilerConfiguration *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_configuration_same_identity(const UmiPerformanceProfilerConfiguration *left, const UmiPerformanceProfilerConfiguration *right);
/* Domain-specific policy helper for profiler configuration. */
double umi_performance_profiler_configuration_weighted_score(double primary, double secondary, double secondary_weight);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_profiler_configuration_archive_encode(const UmiPerformanceProfilerConfiguration *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_profiler_configuration_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceProfilerConfiguration *value);

#ifdef __cplusplus
}
#endif
#endif
