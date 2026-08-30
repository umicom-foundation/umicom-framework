/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/lease_store.c
 *
 * PURPOSE:
 *   Persist edit leases with expiry, renewal and monotonically increasing fencing tokens.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/lease_store.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <stdio.h>
#include <string.h>



static UmiStatus encode_bridge(
    const void *record,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    return umi_workbench_layout_lease_store_encode(
        (const UmiWorkbenchLayoutLease *)record, buffer, capacity, out_required);
}

static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_lease_store_decode(
        value, (UmiWorkbenchLayoutLease *)out_record);
}

UmiStatus umi_workbench_layout_lease_store_encode(
    const UmiWorkbenchLayoutLease *record,
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
            &fields, "lease_id", record->lease_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "layout_id", record->layout_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "holder_id", record->holder_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "client_id", record->client_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "state", (uint32_t)record->state);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "fencing_token", record->fencing_token);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "acquired_at_ms", record->acquired_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "renewed_at_ms", record->renewed_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "expires_at_ms", record->expires_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "revision", record->revision);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

UmiStatus umi_workbench_layout_lease_store_decode(
    const char *value,
    UmiWorkbenchLayoutLease *out_record)
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
            &fields, "lease_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->lease_id, sizeof(out_record->lease_id), text, true)
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
            &fields, "holder_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->holder_id, sizeof(out_record->holder_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "client_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->client_id, sizeof(out_record->client_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    if (status == UMI_STATUS_OK) {
        uint32_t parsed = 0U;
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "state", &parsed);
        out_record->state = (UmiWorkbenchLayoutDataLeaseState)parsed;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "fencing_token", &out_record->fencing_token);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "acquired_at_ms", &out_record->acquired_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "renewed_at_ms", &out_record->renewed_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "expires_at_ms", &out_record->expires_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "revision", &out_record->revision);
    }
    return status;
}

UmiStatus umi_workbench_layout_lease_store_repository_init(
    UmiWorkbenchLayoutLeaseStoreRepository *repository,
    UmiDataServer *server)
{
    UmiStatus status;
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(repository, 0, sizeof(*repository));
    repository->structure_size = sizeof(*repository);
    status = umi_workbench_layout_data_record_repository_init(
        &repository->records,
        server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LEASE,
        sizeof(UmiWorkbenchLayoutLease),
        encode_bridge,
        decode_bridge);
    return status;
}

UmiStatus umi_workbench_layout_lease_store_save(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const UmiWorkbenchLayoutLease *record)
{
    if (repository == NULL || record == NULL ||
        repository->structure_size < sizeof(*repository)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_save(
        &repository->records,
        record->layout_id,
        record->lease_id,
        0U,
        record);
}

UmiStatus umi_workbench_layout_lease_store_load(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutLease *out_record)
{
    if (repository == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_record_repository_load(
        &repository->records, aggregate_id, record_id,
        sequence, out_record);
}

UmiStatus umi_workbench_layout_lease_store_delete(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_data_record_repository_delete(
        &repository->records, aggregate_id, record_id, sequence);
}

UmiStatus umi_workbench_layout_lease_store_list(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutLease *records,
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

UmiStatus umi_workbench_layout_lease_store_acquire(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const char *layout_id,
    const char *holder_id,
    const char *client_id,
    uint64_t now_ms,
    uint64_t duration_ms,
    UmiWorkbenchLayoutLease *out_lease)
{
    UmiWorkbenchLayoutLease existing[UMI_WORKBENCH_LAYOUT_DATA_MAX_LEASES];
    size_t count = 0U;
    size_t index;
    uint64_t maximum_token = 0U;
    UmiStatus status;
    if (repository == NULL || layout_id == NULL ||
        holder_id == NULL || client_id == NULL ||
        duration_ms == 0U || out_lease == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_lease_store_list(
        repository, layout_id, NULL, NULL,
        existing, UMI_WORKBENCH_LAYOUT_DATA_MAX_LEASES,
        &count, NULL);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < count; ++index) {
        if (existing[index].fencing_token > maximum_token) {
            maximum_token = existing[index].fencing_token;
        }
        if (existing[index].state ==
                UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD &&
            !umi_workbench_layout_data_time_expired(
                existing[index].expires_at_ms, now_ms)) {
            return UMI_STATUS_BUSY;
        }
    }
    (void)memset(out_lease, 0, sizeof(*out_lease));
    out_lease->structure_size = sizeof(*out_lease);
    (void)snprintf(out_lease->lease_id,
                   sizeof(out_lease->lease_id),
                   "lease-%s-%020llu", layout_id,
                   (unsigned long long)(maximum_token + 1U));
    (void)umi_workbench_layout_data_copy_text(
        out_lease->layout_id, sizeof(out_lease->layout_id),
        layout_id, false);
    (void)umi_workbench_layout_data_copy_text(
        out_lease->holder_id, sizeof(out_lease->holder_id),
        holder_id, false);
    (void)umi_workbench_layout_data_copy_text(
        out_lease->client_id, sizeof(out_lease->client_id),
        client_id, false);
    out_lease->state = UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD;
    out_lease->fencing_token = maximum_token + 1U;
    out_lease->acquired_at_ms = now_ms;
    out_lease->renewed_at_ms = now_ms;
    out_lease->expires_at_ms = now_ms + duration_ms;
    out_lease->revision = 1U;
    return umi_workbench_layout_lease_store_save(repository, out_lease);
}

UmiStatus umi_workbench_layout_lease_store_renew(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    UmiWorkbenchLayoutLease *lease,
    uint64_t now_ms,
    uint64_t duration_ms)
{
    if (repository == NULL || lease == NULL || duration_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (lease->state != UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD ||
        umi_workbench_layout_data_time_expired(
            lease->expires_at_ms, now_ms)) {
        return UMI_STATUS_INVALID_STATE;
    }
    lease->renewed_at_ms = now_ms;
    lease->expires_at_ms = now_ms + duration_ms;
    lease->revision += 1U;
    return umi_workbench_layout_lease_store_save(repository, lease);
}

UmiStatus umi_workbench_layout_lease_store_release(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    UmiWorkbenchLayoutLease *lease,
    uint64_t now_ms)
{
    if (repository == NULL || lease == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    lease->state = UMI_WORKBENCH_LAYOUT_DATA_LEASE_RELEASED;
    lease->renewed_at_ms = now_ms;
    lease->expires_at_ms = now_ms;
    lease->revision += 1U;
    return umi_workbench_layout_lease_store_save(repository, lease);
}

bool umi_workbench_layout_lease_store_allows_write(
    const UmiWorkbenchLayoutLease *lease,
    const char *holder_id,
    const char *client_id,
    uint64_t fencing_token,
    uint64_t now_ms)
{
    return lease != NULL && holder_id != NULL && client_id != NULL &&
        lease->state == UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD &&
        strcmp(lease->holder_id, holder_id) == 0 &&
        strcmp(lease->client_id, client_id) == 0 &&
        lease->fencing_token == fencing_token &&
        !umi_workbench_layout_data_time_expired(
            lease->expires_at_ms, now_ms);
}
