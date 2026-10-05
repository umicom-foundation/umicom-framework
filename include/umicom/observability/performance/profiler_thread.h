/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_thread.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler thread.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_THREAD_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_THREAD_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance profiler thread data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceProfilerThread {
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
} UmiPerformanceProfilerThread;

/* Initialise a versioned profiler thread record with stable identities. */
UmiStatus umi_performance_profiler_thread_init(UmiPerformanceProfilerThread *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_thread_validate(const UmiPerformanceProfilerThread *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_thread_observe(UmiPerformanceProfilerThread *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_thread_same_identity(const UmiPerformanceProfilerThread *left, const UmiPerformanceProfilerThread *right);
/* Domain-specific policy helper for profiler thread. */
double umi_performance_profiler_thread_weighted_score(double primary, double secondary, double secondary_weight);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_profiler_thread_archive_encode(const UmiPerformanceProfilerThread *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_profiler_thread_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceProfilerThread *value);

#ifdef __cplusplus
}
#endif
#endif
