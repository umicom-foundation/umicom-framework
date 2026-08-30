/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/performance/coverage_summary.c
 *
 * PURPOSE:
 *   Implement represent code-coverage evidence, baselines and regressions for coverage summary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/observability/performance/coverage_summary.h"

/* Initialise deterministic record metadata before any measurement is observed. */
UmiStatus umi_performance_coverage_summary_init(UmiPerformanceCoverageSummary *record, const char *id, const char *subject_id) {
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
UmiStatus umi_performance_coverage_summary_validate(const UmiPerformanceCoverageSummary *record) {
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (record->structure_size != (uint32_t)sizeof(*record) || record->api_version != UMI_PERFORMANCE_API_VERSION) return UMI_STATUS_INVALID_STATE;
    if (!umi_performance_id_valid(record->id) || !umi_performance_id_valid(record->subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if ((unsigned)record->state > (unsigned)UMI_PERFORMANCE_STATE_FAILED || (unsigned)record->severity > (unsigned)UMI_PERFORMANCE_SEVERITY_CRITICAL) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Store scalar evidence without silently changing the caller-selected lifecycle state. */
UmiStatus umi_performance_coverage_summary_observe(UmiPerformanceCoverageSummary *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns) {
    UmiStatus status = umi_performance_coverage_summary_validate(record);
    if (status != UMI_STATUS_OK) return status;
    record->value = value;
    record->auxiliary = auxiliary;
    record->count = count;
    record->timestamp_ns = timestamp_ns;
    ++record->sequence;
    return UMI_STATUS_OK;
}

/* Stable identity is intentionally independent of sequence, severity and measured values. */
bool umi_performance_coverage_summary_same_identity(const UmiPerformanceCoverageSummary *left, const UmiPerformanceCoverageSummary *right) {
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0 && strcmp(left->subject_id, right->subject_id) == 0;
}

/* Convert covered/total evidence into a bounded percentage. */
double umi_performance_coverage_summary_coverage_percent(uint64_t covered, uint64_t total) {
    if (total == 0U) return 0.0;
    if (covered > total) covered = total;
    return ((double)covered / (double)total) * 100.0;
}
