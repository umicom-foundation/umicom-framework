/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_stack.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler stack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_STACK_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_STACK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance profiler stack data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceProfilerStack {
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
} UmiPerformanceProfilerStack;

/* Initialise a versioned profiler stack record with stable identities. */
UmiStatus umi_performance_profiler_stack_init(UmiPerformanceProfilerStack *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_stack_validate(const UmiPerformanceProfilerStack *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_stack_observe(UmiPerformanceProfilerStack *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_stack_same_identity(const UmiPerformanceProfilerStack *left, const UmiPerformanceProfilerStack *right);
/* Domain-specific policy helper for profiler stack. */
double umi_performance_profiler_stack_weighted_score(double primary, double secondary, double secondary_weight);

#ifdef __cplusplus
}
#endif
#endif
