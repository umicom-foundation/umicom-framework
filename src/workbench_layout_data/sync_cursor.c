/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/sync_cursor.c
 *
 * PURPOSE:
 *   Persist per-peer replication cursors and last-success evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/sync_cursor.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <stdio.h>
#include <string.h>



static UmiStatus encode_bridge(
    const void *record,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    return umi_workbench_layout_sync_cursor_encode(
        (const UmiWorkbenchLayoutSyncCursor *)record, buffer, capacity, out_required);
}

static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_sync_cursor_decode(
        value, (UmiWorkbenchLayoutSyncCursor *)out_record);
}

UmiStatus umi_workbench_layout_sync_cursor_encode(
    const UmiWorkbenchLayoutSyncCursor *record,
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
            &fields, "cursor_id", record->cursor_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "replica_id", record->replica_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "peer_id", record->peer_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "last_pulled_sequence", record->last_pulled_sequence);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "last_pushed_sequence", record->last_pushed_sequence);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "last_success_at_ms", record->last_success_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "last_attempt_at_ms", record->last_attempt_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "revision", record->revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "last_error", record->last_error);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

UmiStatus umi_workbench_layout_sync_cursor_decode(
    const char *value,
    UmiWorkbenchLayoutSyncCursor *out_record)
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
            &fields, "cursor_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->cursor_id, sizeof(out_record->cursor_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "replica_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->replica_id, sizeof(out_record->replica_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "peer_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->peer_id, sizeof(out_record->peer_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "last_pulled_sequence", &out_record->last_pulled_sequence);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "last_pushed_sequence", &out_record->last_pushed_sequence);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "last_success_at_ms", &out_record->last_success_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "last_attempt_at_ms", &out_record->last_attempt_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "revision", &out_record->revision);
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

UmiStatus umi_workbench_layout_sync_cursor_repository_init(
    UmiWorkbenchLayoutSyncCursorRepository *repository,
    UmiDataServer *server)
{
    UmiStatus status;
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(repository, 0, sizeof(*repository));
    repository->structure_size = sizeof(*repository);
    status = umi_workbench_layout_data_record_repository_init(
        &repository->records,
        server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR,
        sizeof(UmiWorkbenchLayoutSyncCursor),
        encode_bridge,
        decode_bridge);
    return status;
}

UmiStatus umi_workbench_layout_sync_cursor_save(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const UmiWorkbenchLayoutSyncCursor *record)
{
    if (repository == NULL || record == NULL ||
        repository->structure_size < sizeof(*repository)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_save(
        &repository->records,
        record->replica_id,
        record->cursor_id,
        0U,
        record);
}

UmiStatus umi_workbench_layout_sync_cursor_load(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutSyncCursor *out_record)
{
    if (repository == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_load(
        &repository->records, aggregate_id, record_id,
        sequence, out_record);
}

UmiStatus umi_workbench_layout_sync_cursor_delete(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_data_record_repository_delete(
        &repository->records, aggregate_id, record_id, sequence);
}

UmiStatus umi_workbench_layout_sync_cursor_list(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutSyncCursor *records,
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

UmiStatus umi_workbench_layout_sync_cursor_record_attempt(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    UmiWorkbenchLayoutSyncCursor *cursor,
    uint64_t attempted_at_ms,
    const char *error)
{
    if (repository == NULL || cursor == NULL || error == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    cursor->last_attempt_at_ms = attempted_at_ms;
    cursor->revision += 1U;
    (void)umi_workbench_layout_data_copy_text(
        cursor->last_error, sizeof(cursor->last_error),
        error, true);
    return umi_workbench_layout_sync_cursor_save(repository, cursor);
}

UmiStatus umi_workbench_layout_sync_cursor_record_success(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    UmiWorkbenchLayoutSyncCursor *cursor,
    uint64_t pulled_sequence,
    uint64_t pushed_sequence,
    uint64_t succeeded_at_ms)
{
    if (repository == NULL || cursor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (pulled_sequence > cursor->last_pulled_sequence) {
        cursor->last_pulled_sequence = pulled_sequence;
    }
    if (pushed_sequence > cursor->last_pushed_sequence) {
        cursor->last_pushed_sequence = pushed_sequence;
    }
    cursor->last_success_at_ms = succeeded_at_ms;
    cursor->last_attempt_at_ms = succeeded_at_ms;
    cursor->last_error[0] = '\0';
    cursor->revision += 1U;
    return umi_workbench_layout_sync_cursor_save(repository, cursor);
}
