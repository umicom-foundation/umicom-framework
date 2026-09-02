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



/* Provide the encode bridge operation used by this module and its client applications. */
static UmiStatus encode_bridge(
    const void *record,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    return umi_workbench_layout_sync_cursor_encode(
        (const UmiWorkbenchLayoutSyncCursor *)record, buffer, capacity, out_required);
}

/* Provide the decode bridge operation used by this module and its client applications. */
static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_sync_cursor_decode(
        value, (UmiWorkbenchLayoutSyncCursor *)out_record);
}

/*
 * Write workbench layout sync cursor in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_sync_cursor_encode(
    const UmiWorkbenchLayoutSyncCursor *record,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->structure_size < sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_layout_data_field_set_init(&fields);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "cursor_id", record->cursor_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "replica_id", record->replica_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "peer_id", record->peer_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "last_pulled_sequence", record->last_pulled_sequence);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "last_pushed_sequence", record->last_pushed_sequence);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "last_success_at_ms", record->last_success_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "last_attempt_at_ms", record->last_attempt_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "revision", record->revision);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "last_error", record->last_error);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

/*
 * Read workbench layout sync cursor into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_sync_cursor_decode(
    const char *value,
    UmiWorkbenchLayoutSyncCursor *out_record)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_record, 0, sizeof(*out_record));
    out_record->structure_size = sizeof(*out_record);
    status = umi_workbench_layout_data_value_decode(value, &fields);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "cursor_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->cursor_id, sizeof(out_record->cursor_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "replica_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->replica_id, sizeof(out_record->replica_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "peer_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->peer_id, sizeof(out_record->peer_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "last_pulled_sequence", &out_record->last_pulled_sequence);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "last_pushed_sequence", &out_record->last_pushed_sequence);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "last_success_at_ms", &out_record->last_success_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "last_attempt_at_ms", &out_record->last_attempt_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "revision", &out_record->revision);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Initialise workbench layout sync cursor repository from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_sync_cursor_repository_init(
    UmiWorkbenchLayoutSyncCursorRepository *repository,
    UmiDataServer *server)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Write workbench layout sync cursor in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_sync_cursor_save(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const UmiWorkbenchLayoutSyncCursor *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Read workbench layout sync cursor into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_sync_cursor_load(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutSyncCursor *out_record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_load(
        &repository->records, aggregate_id, record_id,
        sequence, out_record);
}

/*
 * Provide the workbench layout sync cursor delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_sync_cursor_delete(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_data_record_repository_delete(
        &repository->records, aggregate_id, record_id, sequence);
}

/*
 * Provide the workbench layout sync cursor list operation used by this module and its
 * client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count != NULL) *out_count = page.count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_total != NULL) *out_total = page.total_available;
    return status;
}

/*
 * Provide the workbench layout sync cursor record attempt operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_sync_cursor_record_attempt(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    UmiWorkbenchLayoutSyncCursor *cursor,
    uint64_t attempted_at_ms,
    const char *error)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the workbench layout sync cursor record success operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_sync_cursor_record_success(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    UmiWorkbenchLayoutSyncCursor *cursor,
    uint64_t pulled_sequence,
    uint64_t pushed_sequence,
    uint64_t succeeded_at_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || cursor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (pulled_sequence > cursor->last_pulled_sequence) {
        cursor->last_pulled_sequence = pulled_sequence;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (pushed_sequence > cursor->last_pushed_sequence) {
        cursor->last_pushed_sequence = pushed_sequence;
    }
    cursor->last_success_at_ms = succeeded_at_ms;
    cursor->last_attempt_at_ms = succeeded_at_ms;
    cursor->last_error[0] = '\0';
    cursor->revision += 1U;
    return umi_workbench_layout_sync_cursor_save(repository, cursor);
}
