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
UmiStatus umi_performance_profiler_sample_buffer_init(UmiPerformanceProfilerSampleBuffer *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_sample_buffer_validate(const UmiPerformanceProfilerSampleBuffer *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_sample_buffer_observe(UmiPerformanceProfilerSampleBuffer *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_sample_buffer_same_identity(const UmiPerformanceProfilerSampleBuffer *left, const UmiPerformanceProfilerSampleBuffer *right);
/* Domain-specific policy helper for profiler sample buffer. */
bool umi_performance_profiler_sample_buffer_capacity_available(size_t count, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
