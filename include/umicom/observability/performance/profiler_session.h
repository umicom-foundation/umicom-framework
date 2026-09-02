/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/profiler_session.h
 *
 * PURPOSE:
 *   Represent and evaluate reusable CPU/process profiling state for profiler session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_SESSION_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PROFILER_SESSION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance profiler session data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceProfilerSession {
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
} UmiPerformanceProfilerSession;

/* Initialise a versioned profiler session record with stable identities. */
UmiStatus umi_performance_profiler_session_init(UmiPerformanceProfilerSession *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_profiler_session_validate(const UmiPerformanceProfilerSession *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_profiler_session_observe(UmiPerformanceProfilerSession *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_profiler_session_same_identity(const UmiPerformanceProfilerSession *left, const UmiPerformanceProfilerSession *right);
/* Domain-specific policy helper for profiler session. */
uint64_t umi_performance_profiler_session_duration_ns(uint64_t begin_ns, uint64_t end_ns);

#ifdef __cplusplus
}
#endif
#endif
