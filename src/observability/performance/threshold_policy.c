/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/performance/threshold_policy.c
 *
 * PURPOSE:
 *   Implement evaluate reusable observability threshold policy for threshold policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/observability/performance/threshold_policy.h"

/* Initialise deterministic record metadata before any measurement is observed. */
UmiStatus umi_performance_threshold_policy_init(UmiPerformanceThresholdPolicy *record, const char *id, const char *subject_id) {
    UmiStatus status;
    if (record == NULL || id == NULL || subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->api_version = UMI_PERFORMANCE_API_VERSION;
    record->state = UMI_PERFORMANCE_STATE_IDLE;
    record->severity = UMI_PERFORMANCE_SEVERITY_INFO;
    record->enabled = true;
    status = umi_performance_copy_text(record->id, sizeof(record->id), id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_performance_copy_text(record->subject_id, sizeof(record->subject_id), subject_id);
    if (status != UMI_STATUS_OK) { record->id[0] = '\0'; return status; }
    return UMI_STATUS_OK;
}

/* Reject incompatible ABI snapshots and malformed stable identifiers. */
UmiStatus umi_performance_threshold_policy_validate(const UmiPerformanceThresholdPolicy *record) {
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (record->structure_size != (uint32_t)sizeof(*record) || record->api_version != UMI_PERFORMANCE_API_VERSION) return UMI_STATUS_INVALID_STATE;
    if (!umi_performance_id_valid(record->id) || !umi_performance_id_valid(record->subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if ((unsigned)record->state > (unsigned)UMI_PERFORMANCE_STATE_FAILED || (unsigned)record->severity > (unsigned)UMI_PERFORMANCE_SEVERITY_CRITICAL) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Store scalar evidence without silently changing the caller-selected lifecycle state. */
UmiStatus umi_performance_threshold_policy_observe(UmiPerformanceThresholdPolicy *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns) {
    UmiStatus status = umi_performance_threshold_policy_validate(record);
    if (status != UMI_STATUS_OK) return status;
    record->value = value;
    record->auxiliary = auxiliary;
    record->count = count;
    record->timestamp_ns = timestamp_ns;
    ++record->sequence;
    return UMI_STATUS_OK;
}

/* Stable identity is intentionally independent of sequence, severity and measured values. */
bool umi_performance_threshold_policy_same_identity(const UmiPerformanceThresholdPolicy *left, const UmiPerformanceThresholdPolicy *right) {
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0 && strcmp(left->subject_id, right->subject_id) == 0;
}

/* Enforce the common forward lifecycle used by performance observations. */
bool umi_performance_threshold_policy_transition_allowed(UmiPerformanceState from, UmiPerformanceState to) {
    if (from == to) return true;
    if (from == UMI_PERFORMANCE_STATE_IDLE) return to == UMI_PERFORMANCE_STATE_ACTIVE || to == UMI_PERFORMANCE_STATE_FAILED;
    if (from == UMI_PERFORMANCE_STATE_ACTIVE) return to == UMI_PERFORMANCE_STATE_COMPLETE || to == UMI_PERFORMANCE_STATE_FAILED;
    return false;
}
