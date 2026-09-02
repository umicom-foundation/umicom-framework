/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/observability_snapshot.h
 *
 * PURPOSE:
 *   Represent immutable point-in-time observability snapshots for observability snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_OBSERVABILITY_SNAPSHOT_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_OBSERVABILITY_SNAPSHOT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance observability snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceObservabilitySnapshot {
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
} UmiPerformanceObservabilitySnapshot;

/* Initialise a versioned observability snapshot record with stable identities. */
UmiStatus umi_performance_observability_snapshot_init(UmiPerformanceObservabilitySnapshot *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_observability_snapshot_validate(const UmiPerformanceObservabilitySnapshot *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_observability_snapshot_observe(UmiPerformanceObservabilitySnapshot *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_observability_snapshot_same_identity(const UmiPerformanceObservabilitySnapshot *left, const UmiPerformanceObservabilitySnapshot *right);
/* Domain-specific policy helper for observability snapshot. */
uint64_t umi_performance_observability_snapshot_fingerprint(const char *text);

#ifdef __cplusplus
}
#endif
#endif
