/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_sample_buffer.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler sample buffer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_SAMPLE_BUFFER_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_SAMPLE_BUFFER_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance profiler sample buffer data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceProfilerSampleBuffer {
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
} UmiPerformanceProfilerSampleBuffer;

/* Initialise a versioned profiler sample buffer record with stable identities. */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_performance_profiler_sample_buffer_init(UmiPerformanceProfilerSampleBuffer *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_sample_buffer_validate(const UmiPerformanceProfilerSampleBuffer *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_sample_buffer_observe(UmiPerformanceProfilerSampleBuffer *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_sample_buffer_same_identity(const UmiPerformanceProfilerSampleBuffer *left, const UmiPerformanceProfilerSampleBuffer *right);
/* Domain-specific policy helper for profiler sample buffer. */
bool umi_performance_profiler_sample_buffer_capacity_available(size_t count, size_t capacity);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_profiler_sample_buffer_archive_encode(const UmiPerformanceProfilerSampleBuffer *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_profiler_sample_buffer_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceProfilerSampleBuffer *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_performance_profiler_sample_buffer_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPerformanceProfilerSampleBuffer *expected, UmiPerformanceProfilerSampleBuffer *value);


#ifdef __cplusplus
}
#endif
#endif
