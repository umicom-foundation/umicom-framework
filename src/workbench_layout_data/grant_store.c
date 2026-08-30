/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/grant_store.c
 *
 * PURPOSE:
 *   Persist layout sharing grants and evaluate time-bounded permissions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/grant_store.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <stdio.h>
#include <string.h>



static UmiStatus encode_bridge(
    const void *record,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    return umi_workbench_layout_grant_store_encode(
        (const UmiWorkbenchLayoutGrant *)record, buffer, capacity, out_required);
}

static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_grant_store_decode(
        value, (UmiWorkbenchLayoutGrant *)out_record);
}

UmiStatus umi_workbench_layout_grant_store_encode(
    const UmiWorkbenchLayoutGrant *record,
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
            &fields, "grant_id", record->grant_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "layout_id", record->layout_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "subject_id", record->subject_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "granted_by", record->granted_by);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "granted_at_ms", record->granted_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "expires_at_ms", record->expires_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "permissions", record->permissions);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_bool(
            &fields, "inherited", record->inherited);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_bool(
            &fields, "revoked", record->revoked);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

UmiStatus umi_workbench_layout_grant_store_decode(
    const char *value,
    UmiWorkbenchLayoutGrant *out_record)
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
            &fields, "grant_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->grant_id, sizeof(out_record->grant_id), text, true)
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
            &fields, "subject_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->subject_id, sizeof(out_record->subject_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "granted_by");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->granted_by, sizeof(out_record->granted_by), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "granted_at_ms", &out_record->granted_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "expires_at_ms", &out_record->expires_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "permissions", &out_record->permissions);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_bool(
            &fields, "inherited", &out_record->inherited);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_bool(
            &fields, "revoked", &out_record->revoked);
    }
    return status;
}

UmiStatus umi_workbench_layout_grant_store_repository_init(
    UmiWorkbenchLayoutGrantStoreRepository *repository,
    UmiDataServer *server)
{
    UmiStatus status;
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(repository, 0, sizeof(*repository));
    repository->structure_size = sizeof(*repository);
    status = umi_workbench_layout_data_record_repository_init(
        &repository->records,
        server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_GRANT,
        sizeof(UmiWorkbenchLayoutGrant),
        encode_bridge,
        decode_bridge);
    return status;
}

UmiStatus umi_workbench_layout_grant_store_save(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const UmiWorkbenchLayoutGrant *record)
{
    if (repository == NULL || record == NULL ||
        repository->structure_size < sizeof(*repository)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_save(
        &repository->records,
        record->layout_id,
        record->grant_id,
        0U,
        record);
}

UmiStatus umi_workbench_layout_grant_store_load(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutGrant *out_record)
{
    if (repository == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_load(
        &repository->records, aggregate_id, record_id,
        sequence, out_record);
}

UmiStatus umi_workbench_layout_grant_store_delete(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_data_record_repository_delete(
        &repository->records, aggregate_id, record_id, sequence);
}

UmiStatus umi_workbench_layout_grant_store_list(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutGrant *records,
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

typedef struct GrantPredicateContext {
    const char *subject_id;
    UmiWorkbenchLayoutDataPermission permission;
    uint64_t now_ms;
} GrantPredicateContext;

static UmiStatus grant_matches(
    const void *record,
    void *context,
    bool *out_matches)
{
    const UmiWorkbenchLayoutGrant *grant = (const UmiWorkbenchLayoutGrant *)record;
    const GrantPredicateContext *criteria =
        (const GrantPredicateContext *)context;
    const uint32_t mask = UINT32_C(1) << (uint32_t)criteria->permission;
    if (grant == NULL || criteria == NULL || out_matches == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matches =
        !grant->revoked &&
        strcmp(grant->subject_id, criteria->subject_id) == 0 &&
        (grant->permissions & mask) != 0U &&
        !umi_workbench_layout_data_time_expired(
            grant->expires_at_ms, criteria->now_ms);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_grant_store_evaluate(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *layout_id,
    const char *subject_id,
    UmiWorkbenchLayoutDataPermission permission,
    uint64_t now_ms,
    bool *out_allowed)
{
    UmiWorkbenchLayoutGrant grants[UMI_WORKBENCH_LAYOUT_DATA_MAX_GRANTS];
    GrantPredicateContext criteria;
    size_t count = 0U;
    UmiStatus status;
    if (repository == NULL || layout_id == NULL ||
        subject_id == NULL || out_allowed == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_allowed = false;
    criteria.subject_id = subject_id;
    criteria.permission = permission;
    criteria.now_ms = now_ms;
    status = umi_workbench_layout_grant_store_list(
        repository, layout_id, grant_matches, &criteria,
        grants, UMI_WORKBENCH_LAYOUT_DATA_MAX_GRANTS,
        &count, NULL);
    if (status != UMI_STATUS_OK) return status;
    *out_allowed = count > 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_grant_store_revoke(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *layout_id,
    const char *grant_id)
{
    UmiWorkbenchLayoutGrant grant;
    UmiStatus status = umi_workbench_layout_grant_store_load(
        repository, layout_id, grant_id, 0U, &grant);
    if (status != UMI_STATUS_OK) return status;
    grant.revoked = true;
    return umi_workbench_layout_grant_store_save(repository, &grant);
}
