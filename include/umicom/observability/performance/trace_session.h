/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/trace_session.h
 *
 * PURPOSE:
 *   Represent distributed/runtime trace state and filtering policy for trace session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_TRACE_SESSION_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_TRACE_SESSION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance trace session data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceTraceSession {
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
} UmiPerformanceTraceSession;

/* Initialise a versioned trace session record with stable identities. */
UmiStatus umi_performance_trace_session_init(UmiPerformanceTraceSession *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_trace_session_validate(const UmiPerformanceTraceSession *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_trace_session_observe(UmiPerformanceTraceSession *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_trace_session_same_identity(const UmiPerformanceTraceSession *left, const UmiPerformanceTraceSession *right);
/* Domain-specific policy helper for trace session. */
uint64_t umi_performance_trace_session_duration_ns(uint64_t begin_ns, uint64_t end_ns);

#ifdef __cplusplus
}
#endif
#endif
