/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/correlation_context.h
 *
 * PURPOSE:
 *   Carry deterministic correlation state across telemetry domains for correlation context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_CORRELATION_CONTEXT_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_CORRELATION_CONTEXT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance correlation context data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceCorrelationContext {
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
} UmiPerformanceCorrelationContext;

/* Initialise a versioned correlation context record with stable identities. */
UmiStatus umi_performance_correlation_context_init(UmiPerformanceCorrelationContext *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_correlation_context_validate(const UmiPerformanceCorrelationContext *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_correlation_context_observe(UmiPerformanceCorrelationContext *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_correlation_context_same_identity(const UmiPerformanceCorrelationContext *left, const UmiPerformanceCorrelationContext *right);
/* Domain-specific policy helper for correlation context. */
uint64_t umi_performance_correlation_context_fingerprint(const char *text);

#ifdef __cplusplus
}
#endif
#endif
