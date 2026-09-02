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

#ifdef __cplusplus
}
#endif
#endif
