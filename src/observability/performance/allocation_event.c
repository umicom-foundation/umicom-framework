/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/performance/allocation_event.c
 *
 * PURPOSE:
 *   Implement represent memory allocation evidence and allocation aggregation state for allocation event.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/observability/performance/allocation_event.h"

/* Initialise deterministic record metadata before any measurement is observed. */
UmiStatus umi_performance_allocation_event_init(UmiPerformanceAllocationEvent *record, const char *id, const char *subject_id) {
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
UmiStatus umi_performance_allocation_event_validate(const UmiPerformanceAllocationEvent *record) {
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (record->structure_size != (uint32_t)sizeof(*record) || record->api_version != UMI_PERFORMANCE_API_VERSION) return UMI_STATUS_INVALID_STATE;
    if (!umi_performance_id_valid(record->id) || !umi_performance_id_valid(record->subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if ((unsigned)record->state > (unsigned)UMI_PERFORMANCE_STATE_FAILED || (unsigned)record->severity > (unsigned)UMI_PERFORMANCE_SEVERITY_CRITICAL) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Store scalar evidence without silently changing the caller-selected lifecycle state. */
UmiStatus umi_performance_allocation_event_observe(UmiPerformanceAllocationEvent *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns) {
    UmiStatus status = umi_performance_allocation_event_validate(record);
    if (status != UMI_STATUS_OK) return status;
    record->value = value;
    record->auxiliary = auxiliary;
    record->count = count;
    record->timestamp_ns = timestamp_ns;
    ++record->sequence;
    return UMI_STATUS_OK;
}

/* Stable identity is intentionally independent of sequence, severity and measured values. */
bool umi_performance_allocation_event_same_identity(const UmiPerformanceAllocationEvent *left, const UmiPerformanceAllocationEvent *right) {
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0 && strcmp(left->subject_id, right->subject_id) == 0;
}

/* Combine a primary signal with a bounded secondary weight. */
double umi_performance_allocation_event_weighted_score(double primary, double secondary, double secondary_weight) {
    double weight = umi_performance_clamp_unit(secondary_weight);
    return primary + (secondary * weight);
}
