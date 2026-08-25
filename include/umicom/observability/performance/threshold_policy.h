/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/threshold_policy.h
 *
 * PURPOSE:
 *   Evaluate reusable observability threshold policy for threshold policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_THRESHOLD_POLICY_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_THRESHOLD_POLICY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceThresholdPolicy {
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
} UmiPerformanceThresholdPolicy;

/* Initialise a versioned threshold policy record with stable identities. */
UmiStatus umi_performance_threshold_policy_init(UmiPerformanceThresholdPolicy *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_threshold_policy_validate(const UmiPerformanceThresholdPolicy *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_threshold_policy_observe(UmiPerformanceThresholdPolicy *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_threshold_policy_same_identity(const UmiPerformanceThresholdPolicy *left, const UmiPerformanceThresholdPolicy *right);
/* Domain-specific policy helper for threshold policy. */
bool umi_performance_threshold_policy_transition_allowed(UmiPerformanceState from, UmiPerformanceState to);

#ifdef __cplusplus
}
#endif
#endif
