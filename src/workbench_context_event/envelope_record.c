/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/envelope_record.c
 *
 * PURPOSE:
 *   Implement validation, bounded mutation and hashing for the event envelope record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/envelope_record.h"

#include <string.h>

/*
 * Initialise workbench context event envelope record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_envelope_record_init(
    UmiWorkbenchContextEventEnvelopeRecord *record,
    const char *record_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->event_kind = UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION;
    record->context_kind = UMI_CONTEXT_KIND_SELECTION;
    record->priority = UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_NORMAL;
    record->state = UMI_WORKBENCH_CONTEXT_EVENT_CREATED;
    record->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record_id != NULL) {
        (void)umi_workbench_context_event_copy_text(
            record->record_id, sizeof(record->record_id), record_id);
    }
}

/*
 * Check that workbench context event envelope record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_envelope_record_validate(
    const UmiWorkbenchContextEventEnvelopeRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->structure_size != sizeof(*record) ||
        record->record_id[0] == '\0' ||
        record->event_kind <= UMI_WORKBENCH_CONTEXT_EVENT_NONE ||
        record->event_kind > UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION ||
        record->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        record->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context event envelope record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_envelope_record_set_source(
    UmiWorkbenchContextEventEnvelopeRecord *record,
    const char *source_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || source_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->source_id, sizeof(record->source_id), source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context event envelope record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_envelope_record_set_subject(
    UmiWorkbenchContextEventEnvelopeRecord *record,
    const char *subject_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->subject_id, sizeof(record->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context event envelope record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_envelope_record_set_group(
    UmiWorkbenchContextEventEnvelopeRecord *record,
    const char *group_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->group_id, sizeof(record->group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context event envelope record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_envelope_record_set_label(
    UmiWorkbenchContextEventEnvelopeRecord *record,
    const char *label)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->label, sizeof(record->label), label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context event envelope record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_envelope_record_hash(
    const UmiWorkbenchContextEventEnvelopeRecord *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0U;
    hash = umi_workbench_context_event_hash_text(
        hash, record->record_id, sizeof(record->record_id));
    hash = umi_workbench_context_event_hash_text(
        hash, record->source_id, sizeof(record->source_id));
    hash = umi_workbench_context_event_hash_text(
        hash, record->subject_id, sizeof(record->subject_id));
    hash = umi_workbench_context_event_hash_text(
        hash, record->group_id, sizeof(record->group_id));
    hash = umi_workbench_context_event_hash_text(
        hash, record->label, sizeof(record->label));
    hash ^= (uint64_t)record->event_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)record->context_kind;
    hash *= UINT64_C(1099511628211);
    return hash;
}

/*
 * Provide the workbench context event envelope record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_envelope_record_touch(
    UmiWorkbenchContextEventEnvelopeRecord *record,
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
