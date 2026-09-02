/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/correlation_record.c
 *
 * PURPOSE:
 *   Implement bounded mutation, validation and hashing for the selection correlation record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/correlation_record.h"

#include <string.h>

/*
 * Initialise workbench selection correlation record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_correlation_record_init(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *record_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->selection_kind = UMI_WORKBENCH_SELECTION_GENERIC;
    record->activation = UMI_WORKBENCH_SELECTION_ACTIVATION_SELECT;
    record->state = UMI_WORKBENCH_SELECTION_STATE_CREATED;
    record->context_kind = UMI_CONTEXT_KIND_SELECTION;
    record->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record_id != NULL) {
        (void)umi_workbench_selection_copy_text(
            record->record_id, sizeof(record->record_id), record_id);
    }
}

/*
 * Check that workbench selection correlation record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_correlation_record_validate(
    const UmiWorkbenchSelectionCorrelationRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->structure_size != sizeof(*record) ||
        record->record_id[0] == '\0' ||
        record->selection_kind < UMI_WORKBENCH_SELECTION_GENERIC ||
        record->selection_kind > UMI_WORKBENCH_SELECTION_MEDIA ||
        record->activation < UMI_WORKBENCH_SELECTION_ACTIVATION_SELECT ||
        record->activation > UMI_WORKBENCH_SELECTION_ACTIVATION_PREVIEW ||
        record->state < UMI_WORKBENCH_SELECTION_STATE_CREATED ||
        record->state > UMI_WORKBENCH_SELECTION_STATE_STALE ||
        record->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        record->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

#define UMI_SELECTION_SETTER(fn, field) \
UmiStatus fn(UmiWorkbenchSelectionCorrelationRecord *record, const char *value) \
{ \
    UmiStatus status; \
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    status = umi_workbench_selection_copy_text( \
        record->field, sizeof(record->field), value); \
    if (status == UMI_STATUS_OK) ++record->revision; \
    return status; \
}

UMI_SELECTION_SETTER(umi_workbench_selection_correlation_record_set_source, source_id)
UMI_SELECTION_SETTER(umi_workbench_selection_correlation_record_set_subject, subject_id)
UMI_SELECTION_SETTER(umi_workbench_selection_correlation_record_set_secondary, secondary_id)
UMI_SELECTION_SETTER(umi_workbench_selection_correlation_record_set_group, group_id)
UMI_SELECTION_SETTER(umi_workbench_selection_correlation_record_set_label, label)

#undef UMI_SELECTION_SETTER

/*
 * Provide the workbench selection correlation record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_correlation_record_hash(
    const UmiWorkbenchSelectionCorrelationRecord *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0U;
    hash = umi_workbench_selection_hash_text(
        hash, record->record_id, sizeof(record->record_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->source_id, sizeof(record->source_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->subject_id, sizeof(record->subject_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->secondary_id, sizeof(record->secondary_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->group_id, sizeof(record->group_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->label, sizeof(record->label));
    hash ^= (uint64_t)record->selection_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)record->context_kind;
    hash *= UINT64_C(1099511628211);
    return hash;
}

/*
 * Provide the workbench selection correlation record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_correlation_record_touch(
    UmiWorkbenchSelectionCorrelationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}
