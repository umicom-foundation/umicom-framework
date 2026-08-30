/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_sample.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler sample.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_SAMPLE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_SAMPLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceProfilerSample {
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
} UmiPerformanceProfilerSample;

/* Initialise a versioned profiler sample record with stable identities. */
UmiStatus umi_performance_profiler_sample_init(UmiPerformanceProfilerSample *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_sample_validate(const UmiPerformanceProfilerSample *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_sample_observe(UmiPerformanceProfilerSample *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_sample_same_identity(const UmiPerformanceProfilerSample *left, const UmiPerformanceProfilerSample *right);
/* Domain-specific policy helper for profiler sample. */
bool umi_performance_profiler_sample_exceeds(double value, double threshold);

#ifdef __cplusplus
}
#endif
#endif
