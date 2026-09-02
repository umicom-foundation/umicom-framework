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



/* Provide the encode bridge operation used by this module and its client applications. */
static UmiStatus encode_bridge(
    const void *record,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    return umi_workbench_layout_lease_store_encode(
        (const UmiWorkbenchLayoutLease *)record, buffer, capacity, out_required);
}

/* Provide the decode bridge operation used by this module and its client applications. */
static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_lease_store_decode(
        value, (UmiWorkbenchLayoutLease *)out_record);
}

/*
 * Write workbench layout lease store in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_lease_store_encode(
    const UmiWorkbenchLayoutLease *record,
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
            &fields, "lease_id", record->lease_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "layout_id", record->layout_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "holder_id", record->holder_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "client_id", record->client_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "state", (uint32_t)record->state);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "fencing_token", record->fencing_token);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "acquired_at_ms", record->acquired_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "renewed_at_ms", record->renewed_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "expires_at_ms", record->expires_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "revision", record->revision);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

/*
 * Read workbench layout lease store into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_lease_store_decode(
    const char *value,
    UmiWorkbenchLayoutLease *out_record)
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
            &fields, "lease_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->lease_id, sizeof(out_record->lease_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "layout_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->layout_id, sizeof(out_record->layout_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "holder_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->holder_id, sizeof(out_record->holder_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "client_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->client_id, sizeof(out_record->client_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        uint32_t parsed = 0U;
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "state", &parsed);
        out_record->state = (UmiWorkbenchLayoutDataLeaseState)parsed;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "fencing_token", &out_record->fencing_token);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "acquired_at_ms", &out_record->acquired_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "renewed_at_ms", &out_record->renewed_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "expires_at_ms", &out_record->expires_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "revision", &out_record->revision);
    }
    return status;
}

/*
 * Initialise workbench layout lease store repository from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_lease_store_repository_init(
    UmiWorkbenchLayoutLeaseStoreRepository *repository,
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
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LEASE,
        sizeof(UmiWorkbenchLayoutLease),
        encode_bridge,
        decode_bridge);
    return status;
}

/*
 * Write workbench layout lease store in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_lease_store_save(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const UmiWorkbenchLayoutLease *record)
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
        record->layout_id,
        record->lease_id,
        0U,
        record);
}

/*
 * Read workbench layout lease store into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_lease_store_load(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutLease *out_record)
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
 * Provide the workbench layout lease store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_lease_store_delete(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
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
 * Provide the workbench layout lease store list operation used by this module and its
 * client applications.
 */
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
 * Provide the workbench layout lease store acquire operation used by this module and its
 * client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || layout_id == NULL ||
        holder_id == NULL || client_id == NULL ||
        duration_ms == 0U || out_lease == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_lease_store_list(
        repository, layout_id, NULL, NULL,
        existing, UMI_WORKBENCH_LAYOUT_DATA_MAX_LEASES,
        &count, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (existing[index].fencing_token > maximum_token) {
            maximum_token = existing[index].fencing_token;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the workbench layout lease store renew operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_lease_store_renew(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    UmiWorkbenchLayoutLease *lease,
    uint64_t now_ms,
    uint64_t duration_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || lease == NULL || duration_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Release or reset state held by workbench layout lease store so the same storage can be
 * reused safely.
 */
UmiStatus umi_workbench_layout_lease_store_release(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    UmiWorkbenchLayoutLease *lease,
    uint64_t now_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || lease == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    lease->state = UMI_WORKBENCH_LAYOUT_DATA_LEASE_RELEASED;
    lease->renewed_at_ms = now_ms;
    lease->expires_at_ms = now_ms;
    lease->revision += 1U;
    return umi_workbench_layout_lease_store_save(repository, lease);
}

/*
 * Write workbench layout lease store allows in its stable representation and report
 * capacity or input failures to the caller.
 */
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
