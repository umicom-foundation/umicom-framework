/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_capability.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_CAPABILITY_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_CAPABILITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance profiler capability data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceProfilerCapability {
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
} UmiPerformanceProfilerCapability;

/* Initialise a versioned profiler capability record with stable identities. */
UmiStatus umi_performance_profiler_capability_init(UmiPerformanceProfilerCapability *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_capability_validate(const UmiPerformanceProfilerCapability *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_capability_observe(UmiPerformanceProfilerCapability *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_capability_same_identity(const UmiPerformanceProfilerCapability *left, const UmiPerformanceProfilerCapability *right);
/* Domain-specific policy helper for profiler capability. */
bool umi_performance_profiler_capability_transition_allowed(UmiPerformanceState from, UmiPerformanceState to);

#ifdef __cplusplus
}
#endif
#endif
