/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_filter.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler filter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_FILTER_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_FILTER_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance profiler filter data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceProfilerFilter {
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
} UmiPerformanceProfilerFilter;

/* Initialise a versioned profiler filter record with stable identities. */
UmiStatus umi_performance_profiler_filter_init(UmiPerformanceProfilerFilter *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_filter_validate(const UmiPerformanceProfilerFilter *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_filter_observe(UmiPerformanceProfilerFilter *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_filter_same_identity(const UmiPerformanceProfilerFilter *left, const UmiPerformanceProfilerFilter *right);
/* Domain-specific policy helper for profiler filter. */
bool umi_performance_profiler_filter_transition_allowed(UmiPerformanceState from, UmiPerformanceState to);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_profiler_filter_archive_encode(const UmiPerformanceProfilerFilter *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_profiler_filter_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceProfilerFilter *value);

#ifdef __cplusplus
}
#endif
#endif
