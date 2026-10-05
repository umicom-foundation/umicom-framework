/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_hotpath.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler hotpath.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_HOTPATH_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_HOTPATH_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance profiler hotpath data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceProfilerHotpath {
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
} UmiPerformanceProfilerHotpath;

/* Initialise a versioned profiler hotpath record with stable identities. */
UmiStatus umi_performance_profiler_hotpath_init(UmiPerformanceProfilerHotpath *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_hotpath_validate(const UmiPerformanceProfilerHotpath *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_hotpath_observe(UmiPerformanceProfilerHotpath *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_hotpath_same_identity(const UmiPerformanceProfilerHotpath *left, const UmiPerformanceProfilerHotpath *right);
/* Domain-specific policy helper for profiler hotpath. */
bool umi_performance_profiler_hotpath_exceeds(double value, double threshold);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_profiler_hotpath_archive_encode(const UmiPerformanceProfilerHotpath *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_profiler_hotpath_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceProfilerHotpath *value);

#ifdef __cplusplus
}
#endif
#endif
