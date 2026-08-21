/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/conflict_store.c
 *
 * PURPOSE:
 *   Persist synchronisation conflicts and explicit human or policy-driven resolution evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/conflict_store.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <stdio.h>
#include <string.h>



static UmiStatus encode_bridge(
    const void *record,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    return umi_workbench_layout_conflict_store_encode(
        (const UmiWorkbenchLayoutConflict *)record, buffer, capacity, out_required);
}

static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_conflict_store_decode(
        value, (UmiWorkbenchLayoutConflict *)out_record);
}

UmiStatus umi_workbench_layout_conflict_store_encode(
    const UmiWorkbenchLayoutConflict *record,
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
            &fields, "conflict_id", record->conflict_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "layout_id", record->layout_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "local_replica_id", record->local_replica_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "remote_replica_id", record->remote_replica_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "kind", (uint32_t)record->kind);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "state", (uint32_t)record->state);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "base_revision", record->base_revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "local_revision", record->local_revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "remote_revision", record->remote_revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "created_at_ms", record->created_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "resolved_at_ms", record->resolved_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "resolved_by", record->resolved_by);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "message", record->message);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

UmiStatus umi_workbench_layout_conflict_store_decode(
    const char *value,
    UmiWorkbenchLayoutConflict *out_record)
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
            &fields, "conflict_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->conflict_id, sizeof(out_record->conflict_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "layout_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->layout_id, sizeof(out_record->layout_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "local_replica_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->local_replica_id, sizeof(out_record->local_replica_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "remote_replica_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->remote_replica_id, sizeof(out_record->remote_replica_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        uint32_t parsed = 0U;
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "kind", &parsed);
        out_record->kind = (UmiWorkbenchLayoutDataConflictKind)parsed;
    }
    if (status == UMI_STATUS_OK) {
        uint32_t parsed = 0U;
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "state", &parsed);
        out_record->state = (UmiWorkbenchLayoutDataConflictState)parsed;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "base_revision", &out_record->base_revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "local_revision", &out_record->local_revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "remote_revision", &out_record->remote_revision);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "created_at_ms", &out_record->created_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "resolved_at_ms", &out_record->resolved_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "resolved_by");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->resolved_by, sizeof(out_record->resolved_by), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "message");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->message, sizeof(out_record->message), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    return status;
}

UmiStatus umi_workbench_layout_conflict_store_repository_init(
    UmiWorkbenchLayoutConflictStoreRepository *repository,
    UmiDataServer *server)
{
    UmiStatus status;
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(repository, 0, sizeof(*repository));
    repository->structure_size = sizeof(*repository);
    status = umi_workbench_layout_data_record_repository_init(
        &repository->records,
        server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_CONFLICT,
        sizeof(UmiWorkbenchLayoutConflict),
        encode_bridge,
        decode_bridge);
    return status;
}

UmiStatus umi_workbench_layout_conflict_store_save(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const UmiWorkbenchLayoutConflict *record)
{
    if (repository == NULL || record == NULL ||
        repository->structure_size < sizeof(*repository)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_save(
        &repository->records,
        record->layout_id,
        record->conflict_id,
        0U,
        record);
}

UmiStatus umi_workbench_layout_conflict_store_load(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutConflict *out_record)
{
    if (repository == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_load(
        &repository->records, aggregate_id, record_id,
        sequence, out_record);
}

UmiStatus umi_workbench_layout_conflict_store_delete(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_data_record_repository_delete(
        &repository->records, aggregate_id, record_id, sequence);
}

UmiStatus umi_workbench_layout_conflict_store_list(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutConflict *records,
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

UmiStatus umi_workbench_layout_conflict_store_resolve(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    UmiWorkbenchLayoutConflict *conflict,
    UmiWorkbenchLayoutDataConflictState resolution,
    const char *resolved_by,
    uint64_t resolved_at_ms)
{
    if (repository == NULL || conflict == NULL ||
        resolved_by == NULL ||
        resolution == UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    conflict->state = resolution;
    conflict->resolved_at_ms = resolved_at_ms;
    (void)umi_workbench_layout_data_copy_text(
        conflict->resolved_by, sizeof(conflict->resolved_by),
        resolved_by, false);
    return umi_workbench_layout_conflict_store_save(repository, conflict);
}

static UmiStatus open_predicate(
    const void *record,
    void *context,
    bool *out_matches)
{
    const UmiWorkbenchLayoutConflict *conflict = (const UmiWorkbenchLayoutConflict *)record;
    (void)context;
    if (conflict == NULL || out_matches == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matches = conflict->state ==
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN;
    return UMI_STATUS_OK;
}

size_t umi_workbench_layout_conflict_store_open_count(
    const UmiWorkbenchLayoutConflictStoreRepository *repository)
{
    UmiWorkbenchLayoutConflict conflicts[UMI_WORKBENCH_LAYOUT_DATA_MAX_CONFLICTS];
    size_t count = 0U;
    if (repository == NULL) return 0U;
    if (umi_workbench_layout_conflict_store_list(
            repository, NULL, open_predicate, NULL,
            conflicts, UMI_WORKBENCH_LAYOUT_DATA_MAX_CONFLICTS,
            &count, NULL) != UMI_STATUS_OK) {
        return 0U;
    }
    return count;
}
