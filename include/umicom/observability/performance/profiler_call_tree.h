/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_call_tree.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler call tree.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_CALL_TREE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_CALL_TREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceProfilerCallTree {
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
} UmiPerformanceProfilerCallTree;

/* Initialise a versioned profiler call tree record with stable identities. */
UmiStatus umi_performance_profiler_call_tree_init(UmiPerformanceProfilerCallTree *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_call_tree_validate(const UmiPerformanceProfilerCallTree *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_call_tree_observe(UmiPerformanceProfilerCallTree *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_call_tree_same_identity(const UmiPerformanceProfilerCallTree *left, const UmiPerformanceProfilerCallTree *right);
/* Domain-specific policy helper for profiler call tree. */
double umi_performance_profiler_call_tree_weighted_score(double primary, double secondary, double secondary_weight);

#ifdef __cplusplus
}
#endif
#endif
