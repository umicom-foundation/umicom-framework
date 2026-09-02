/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/migration_store.c
 *
 * PURPOSE:
 *   Persist schema migration state and evidence for safe restart and rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/migration_store.h"
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
    return umi_workbench_layout_migration_store_encode(
        (const UmiWorkbenchLayoutMigrationRecord *)record, buffer, capacity, out_required);
}

/* Provide the decode bridge operation used by this module and its client applications. */
static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_migration_store_decode(
        value, (UmiWorkbenchLayoutMigrationRecord *)out_record);
}

/*
 * Write workbench layout migration store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_migration_store_encode(
    const UmiWorkbenchLayoutMigrationRecord *record,
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
            &fields, "descriptor_migration_id", record->descriptor.migration_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "descriptor_name", record->descriptor.name);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "descriptor_from_version", record->descriptor.from_version);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "descriptor_to_version", record->descriptor.to_version);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "descriptor_checksum", record->descriptor.checksum);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_bool(
            &fields, "descriptor_reversible", record->descriptor.reversible);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_bool(
            &fields, "descriptor_requires_exclusive_access", record->descriptor.requires_exclusive_access);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "state", (uint32_t)record->state);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "applied_by", record->applied_by);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "started_at_ms", record->started_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "completed_at_ms", record->completed_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "error", record->error);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

/*
 * Read workbench layout migration store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_migration_store_decode(
    const char *value,
    UmiWorkbenchLayoutMigrationRecord *out_record)
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
            &fields, "descriptor_migration_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->descriptor.migration_id, sizeof(out_record->descriptor.migration_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "descriptor_name");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->descriptor.name, sizeof(out_record->descriptor.name), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "descriptor_from_version", &out_record->descriptor.from_version);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "descriptor_to_version", &out_record->descriptor.to_version);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "descriptor_checksum", &out_record->descriptor.checksum);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_bool(
            &fields, "descriptor_reversible", &out_record->descriptor.reversible);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_bool(
            &fields, "descriptor_requires_exclusive_access", &out_record->descriptor.requires_exclusive_access);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        uint32_t parsed = 0U;
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "state", &parsed);
        out_record->state = (UmiWorkbenchLayoutDataMigrationState)parsed;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "applied_by");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->applied_by, sizeof(out_record->applied_by), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "started_at_ms", &out_record->started_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "completed_at_ms", &out_record->completed_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "error");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->error, sizeof(out_record->error), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    return status;
}

/*
 * Initialise workbench layout migration store repository from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_workbench_layout_migration_store_repository_init(
    UmiWorkbenchLayoutMigrationStoreRepository *repository,
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
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_MIGRATION,
        sizeof(UmiWorkbenchLayoutMigrationRecord),
        encode_bridge,
        decode_bridge);
    return status;
}

/*
 * Write workbench layout migration store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_migration_store_save(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const UmiWorkbenchLayoutMigrationRecord *record)
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
        NULL,
        record->descriptor.migration_id,
        0U,
        record);
}

/*
 * Read workbench layout migration store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_migration_store_load(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutMigrationRecord *out_record)
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
 * Provide the workbench layout migration store delete operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_migration_store_delete(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
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
 * Provide the workbench layout migration store list operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_migration_store_list(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutMigrationRecord *records,
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
