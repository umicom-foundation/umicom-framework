/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/producer_record.c
 *
 * PURPOSE:
 *   Implement validation, bounded mutation and hashing for the event producer record.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/producer_record.h"

#include <string.h>

void umi_workbench_context_event_producer_record_init(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *record_id)
{
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->event_kind = UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION;
    record->context_kind = UMI_CONTEXT_KIND_SELECTION;
    record->priority = UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_NORMAL;
    record->state = UMI_WORKBENCH_CONTEXT_EVENT_CREATED;
    record->revision = 1U;
    if (record_id != NULL) {
        (void)umi_workbench_context_event_copy_text(
            record->record_id, sizeof(record->record_id), record_id);
    }
}

UmiStatus umi_workbench_context_event_producer_record_validate(
    const UmiWorkbenchContextEventProducerRecord *record)
{
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

UmiStatus umi_workbench_context_event_producer_record_set_source(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *source_id)
{
    UmiStatus status;
    if (record == NULL || source_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->source_id, sizeof(record->source_id), source_id);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

UmiStatus umi_workbench_context_event_producer_record_set_subject(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *subject_id)
{
    UmiStatus status;
    if (record == NULL || subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->subject_id, sizeof(record->subject_id), subject_id);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

UmiStatus umi_workbench_context_event_producer_record_set_group(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *group_id)
{
    UmiStatus status;
    if (record == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->group_id, sizeof(record->group_id), group_id);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

UmiStatus umi_workbench_context_event_producer_record_set_label(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *label)
{
    UmiStatus status;
    if (record == NULL || label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->label, sizeof(record->label), label);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

uint64_t umi_workbench_context_event_producer_record_hash(
    const UmiWorkbenchContextEventProducerRecord *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
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

void umi_workbench_context_event_producer_record_touch(
    UmiWorkbenchContextEventProducerRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}
