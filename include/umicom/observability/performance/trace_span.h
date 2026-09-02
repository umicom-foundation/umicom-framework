/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/trace_span.h
 *
 * PURPOSE:
 *   Represent distributed/runtime trace state and filtering policy for trace span.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_TRACE_SPAN_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_TRACE_SPAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance trace span data shared with callers of this public contract.
 */
typedef struct UmiPerformanceTraceSpan {
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
} UmiPerformanceTraceSpan;

/* Initialise a versioned trace span record with stable identities. */
UmiStatus umi_performance_trace_span_init(UmiPerformanceTraceSpan *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_trace_span_validate(const UmiPerformanceTraceSpan *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_trace_span_observe(UmiPerformanceTraceSpan *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_trace_span_same_identity(const UmiPerformanceTraceSpan *left, const UmiPerformanceTraceSpan *right);
/* Domain-specific policy helper for trace span. */
uint64_t umi_performance_trace_span_duration_ns(uint64_t begin_ns, uint64_t end_ns);

#ifdef __cplusplus
}
#endif
#endif
