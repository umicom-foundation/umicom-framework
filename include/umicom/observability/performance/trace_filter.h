/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/trace_filter.h
 *
 * PURPOSE:
 *   Represent distributed/runtime trace state and filtering policy for trace filter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_TRACE_FILTER_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_TRACE_FILTER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance trace filter data shared with callers of this public contract.
 */
typedef struct UmiPerformanceTraceFilter {
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
} UmiPerformanceTraceFilter;

/* Initialise a versioned trace filter record with stable identities. */
UmiStatus umi_performance_trace_filter_init(UmiPerformanceTraceFilter *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_trace_filter_validate(const UmiPerformanceTraceFilter *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_trace_filter_observe(UmiPerformanceTraceFilter *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_trace_filter_same_identity(const UmiPerformanceTraceFilter *left, const UmiPerformanceTraceFilter *right);
/* Domain-specific policy helper for trace filter. */
bool umi_performance_trace_filter_transition_allowed(UmiPerformanceState from, UmiPerformanceState to);

#ifdef __cplusplus
}
#endif
#endif
