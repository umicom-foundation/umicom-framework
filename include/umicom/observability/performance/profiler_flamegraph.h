/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_flamegraph.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler flamegraph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_FLAMEGRAPH_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_FLAMEGRAPH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceProfilerFlamegraph {
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
} UmiPerformanceProfilerFlamegraph;

/* Initialise a versioned profiler flamegraph record with stable identities. */
UmiStatus umi_performance_profiler_flamegraph_init(UmiPerformanceProfilerFlamegraph *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_flamegraph_validate(const UmiPerformanceProfilerFlamegraph *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_flamegraph_observe(UmiPerformanceProfilerFlamegraph *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_flamegraph_same_identity(const UmiPerformanceProfilerFlamegraph *left, const UmiPerformanceProfilerFlamegraph *right);
/* Domain-specific policy helper for profiler flamegraph. */
double umi_performance_profiler_flamegraph_weighted_score(double primary, double secondary, double secondary_weight);

#ifdef __cplusplus
}
#endif
#endif
