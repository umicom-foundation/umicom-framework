/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/outbox.c
 *
 * PURPOSE:
 *   Persist transactional outbox work with delivery leases, retries and dead-letter evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/outbox.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <stdio.h>
#include <string.h>



static UmiStatus encode_bridge(
    const void *record,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    return umi_workbench_layout_outbox_encode(
        (const UmiWorkbenchLayoutOutboxEntry *)record, buffer, capacity, out_required);
}

static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_outbox_decode(
        value, (UmiWorkbenchLayoutOutboxEntry *)out_record);
}

UmiStatus umi_workbench_layout_outbox_encode(
    const UmiWorkbenchLayoutOutboxEntry *record,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || record->structure_size < sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_layout_data_field_set_init(&fields);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "entry_id", record->entry_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "topic", record->topic);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "aggregate_id", record->aggregate_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "correlation_id", record->correlation_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "state", (uint32_t)record->state);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "sequence", record->sequence);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "created_at_ms", record->created_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "available_at_ms", record->available_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "leased_until_ms", record->leased_until_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "attempt_count", record->attempt_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "maximum_attempts", record->maximum_attempts);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "payload_reference", record->payload_reference);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "last_error", record->last_error);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

UmiStatus umi_workbench_layout_outbox_decode(
    const char *value,
    UmiWorkbenchLayoutOutboxEntry *out_record)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    UmiStatus status;
    if (value == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_record, 0, sizeof(*out_record));
    out_record->structure_size = sizeof(*out_record);
    status = umi_workbench_layout_data_value_decode(value, &fields);
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "entry_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->entry_id, sizeof(out_record->entry_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "topic");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->topic, sizeof(out_record->topic), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "aggregate_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->aggregate_id, sizeof(out_record->aggregate_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "correlation_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->correlation_id, sizeof(out_record->correlation_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        uint32_t parsed = 0U;
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "state", &parsed);
        out_record->state = (UmiWorkbenchLayoutDataOutboxState)parsed;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "sequence", &out_record->sequence);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "created_at_ms", &out_record->created_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "available_at_ms", &out_record->available_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "leased_until_ms", &out_record->leased_until_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "attempt_count", &out_record->attempt_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "maximum_attempts", &out_record->maximum_attempts);
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "payload_reference");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->payload_reference, sizeof(out_record->payload_reference), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "last_error");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->last_error, sizeof(out_record->last_error), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    return status;
}

UmiStatus umi_workbench_layout_outbox_repository_init(
    UmiWorkbenchLayoutOutboxRepository *repository,
    UmiDataServer *server)
{
    UmiStatus status;
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(repository, 0, sizeof(*repository));
    repository->structure_size = sizeof(*repository);
    status = umi_workbench_layout_data_record_repository_init(
        &repository->records,
        server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX,
        sizeof(UmiWorkbenchLayoutOutboxEntry),
        encode_bridge,
        decode_bridge);
    return status;
}

UmiStatus umi_workbench_layout_outbox_save(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    const UmiWorkbenchLayoutOutboxEntry *record)
{
    if (repository == NULL || record == NULL ||
        repository->structure_size < sizeof(*repository)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_save(
        &repository->records,
        NULL,
        record->entry_id,
        record->sequence,
        record);
}

UmiStatus umi_workbench_layout_outbox_load(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutOutboxEntry *out_record)
{
    if (repository == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_load(
        &repository->records, aggregate_id, record_id,
        sequence, out_record);
}

UmiStatus umi_workbench_layout_outbox_delete(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_data_record_repository_delete(
        &repository->records, aggregate_id, record_id, sequence);
}

UmiStatus umi_workbench_layout_outbox_list(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutOutboxEntry *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total)
{
    UmiWorkbenchLayoutDataRecordPage page;
    UmiStatus status;
    if (repository == NULL || records == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&page, 0, sizeof(page));
    page.structure_size = sizeof(page);
    page.records = records;
    page.record_size = sizeof(records[0]);
    page.capacity = capacity;
    status = umi_workbench_layout_data_record_repository_list(
        &repository->records, aggregate_id, predicate,
        predicate_context, &page);
    if (out_count != NULL) *out_count = page.count;
    if (out_total != NULL) *out_total = page.total_available;
    return status;
}

typedef struct AvailablePredicateContext {
    uint64_t now_ms;
} AvailablePredicateContext;

static UmiStatus available_predicate(
    const void *record,
    void *context,
    bool *out_matches)
{
    const UmiWorkbenchLayoutOutboxEntry *entry = (const UmiWorkbenchLayoutOutboxEntry *)record;
    const AvailablePredicateContext *criteria =
        (const AvailablePredicateContext *)context;
    if (entry == NULL || criteria == NULL || out_matches == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matches =
        (entry->state == UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_PENDING ||
         entry->state == UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_RETRY_WAIT) &&
        entry->available_at_ms <= criteria->now_ms;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_outbox_claim_available(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    uint64_t now_ms,
    uint64_t lease_duration_ms,
    UmiWorkbenchLayoutOutboxEntry *out_entry)
{
    UmiWorkbenchLayoutOutboxEntry available[32];
    AvailablePredicateContext criteria;
    size_t count = 0U;
    UmiStatus status;
    if (repository == NULL || out_entry == NULL ||
        lease_duration_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    criteria.now_ms = now_ms;
    status = umi_workbench_layout_outbox_list(
        repository, NULL, available_predicate, &criteria,
        available, 32U, &count, NULL);
    if (status != UMI_STATUS_OK) return status;
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
    *out_entry = available[0];
    out_entry->state = UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_LEASED;
    out_entry->leased_until_ms = now_ms + lease_duration_ms;
    out_entry->attempt_count += 1U;
    return umi_workbench_layout_outbox_save(repository, out_entry);
}

UmiStatus umi_workbench_layout_outbox_mark_delivered(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    UmiWorkbenchLayoutOutboxEntry *entry, uint64_t delivered_at_ms)
{
    if (repository == NULL || entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    entry->state = UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DELIVERED;
    entry->leased_until_ms = delivered_at_ms;
    return umi_workbench_layout_outbox_save(repository, entry);
}

UmiStatus umi_workbench_layout_outbox_mark_failed(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    UmiWorkbenchLayoutOutboxEntry *entry,
    uint64_t now_ms,
    uint64_t retry_delay_ms,
    const char *error)
{
    UmiWorkbenchLayoutOutboxEntry updated;
    UmiStatus status;
    if (repository == NULL || entry == NULL || error == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    updated = *entry;
    status = umi_workbench_layout_data_copy_text(
        updated.last_error, sizeof(updated.last_error),
        error, true);
    if (status != UMI_STATUS_OK) return status;
    updated.leased_until_ms = 0U;
    if (updated.attempt_count >= updated.maximum_attempts) {
        updated.state = UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DEAD_LETTER;
        updated.available_at_ms = 0U;
    } else {
        updated.state = UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_RETRY_WAIT;
        updated.available_at_ms = now_ms + retry_delay_ms;
    }
    status = umi_workbench_layout_outbox_save(repository, &updated);
    if (status == UMI_STATUS_OK) *entry = updated;
    return status;
}
