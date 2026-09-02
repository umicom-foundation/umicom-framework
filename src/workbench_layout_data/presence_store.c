/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/presence_store.c
 *
 * PURPOSE:
 *   Persist short-lived collaboration presence and active-editor heartbeats.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/presence_store.h"
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
    return umi_workbench_layout_presence_store_encode(
        (const UmiWorkbenchLayoutPresence *)record, buffer, capacity, out_required);
}

/* Provide the decode bridge operation used by this module and its client applications. */
static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_presence_store_decode(
        value, (UmiWorkbenchLayoutPresence *)out_record);
}

/*
 * Write workbench layout presence store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_presence_store_encode(
    const UmiWorkbenchLayoutPresence *record,
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
            &fields, "presence_id", record->presence_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "layout_id", record->layout_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "user_id", record->user_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "client_id", record->client_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "active_node_id", record->active_node_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "connected_at_ms", record->connected_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "last_seen_at_ms", record->last_seen_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "expires_at_ms", record->expires_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_bool(
            &fields, "editing", record->editing);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_bool(
            &fields, "synchronising", record->synchronising);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

/*
 * Read workbench layout presence store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_presence_store_decode(
    const char *value,
    UmiWorkbenchLayoutPresence *out_record)
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
            &fields, "presence_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->presence_id, sizeof(out_record->presence_id), text, true)
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
            &fields, "user_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->user_id, sizeof(out_record->user_id), text, true)
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
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "active_node_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->active_node_id, sizeof(out_record->active_node_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "connected_at_ms", &out_record->connected_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "last_seen_at_ms", &out_record->last_seen_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "expires_at_ms", &out_record->expires_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_bool(
            &fields, "editing", &out_record->editing);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_bool(
            &fields, "synchronising", &out_record->synchronising);
    }
    return status;
}

/*
 * Initialise workbench layout presence store repository from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_workbench_layout_presence_store_repository_init(
    UmiWorkbenchLayoutPresenceStoreRepository *repository,
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
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE,
        sizeof(UmiWorkbenchLayoutPresence),
        encode_bridge,
        decode_bridge);
    return status;
}

/*
 * Write workbench layout presence store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_presence_store_save(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const UmiWorkbenchLayoutPresence *record)
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
        record->presence_id,
        0U,
        record);
}

/*
 * Read workbench layout presence store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_presence_store_load(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutPresence *out_record)
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
 * Provide the workbench layout presence store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_presence_store_delete(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
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
 * Provide the workbench layout presence store list operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_presence_store_list(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutPresence *records,
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
 * Provide the workbench layout presence store heartbeat operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_presence_store_heartbeat(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    UmiWorkbenchLayoutPresence *presence,
    const char *active_node_id,
    bool editing,
    bool synchronising,
    uint64_t now_ms,
    uint64_t ttl_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || presence == NULL ||
        active_node_id == NULL || ttl_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_workbench_layout_data_copy_text(
        presence->active_node_id, sizeof(presence->active_node_id),
        active_node_id, true);
    presence->editing = editing;
    presence->synchronising = synchronising;
    presence->last_seen_at_ms = now_ms;
    presence->expires_at_ms = now_ms + ttl_ms;
    return umi_workbench_layout_presence_store_save(repository, presence);
}

typedef struct PresenceTimeContext {
    uint64_t now_ms;
    bool expired;
} PresenceTimeContext;

/*
 * Provide the presence predicate operation used by this module and its client
 * applications.
 */
static UmiStatus presence_predicate(
    const void *record,
    void *context,
    bool *out_matches)
{
    const UmiWorkbenchLayoutPresence *presence = (const UmiWorkbenchLayoutPresence *)record;
    const PresenceTimeContext *time =
        (const PresenceTimeContext *)context;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (presence == NULL || time == NULL || out_matches == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matches = umi_workbench_layout_data_time_expired(
        presence->expires_at_ms, time->now_ms) == time->expired;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout presence store expire operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_presence_store_expire(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    uint64_t now_ms,
    size_t *out_expired)
{
    UmiWorkbenchLayoutPresence records[UMI_WORKBENCH_LAYOUT_DATA_MAX_PRESENCE];
    PresenceTimeContext context;
    size_t count = 0U;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context.now_ms = now_ms;
    context.expired = true;
    status = umi_workbench_layout_presence_store_list(
        repository, NULL, presence_predicate, &context,
        records, UMI_WORKBENCH_LAYOUT_DATA_MAX_PRESENCE,
        &count, NULL);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        status = umi_workbench_layout_presence_store_delete(
            repository, records[index].layout_id,
            records[index].presence_id, 0U);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_expired != NULL) *out_expired = index;
    return status;
}

/*
 * Return the number of records represented by workbench layout presence store active
 * without changing their state.
 */
size_t umi_workbench_layout_presence_store_active_count(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    uint64_t now_ms)
{
    UmiWorkbenchLayoutPresence records[UMI_WORKBENCH_LAYOUT_DATA_MAX_PRESENCE];
    PresenceTimeContext context;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return 0U;
    context.now_ms = now_ms;
    context.expired = false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_presence_store_list(
            repository, NULL, presence_predicate, &context,
            records, UMI_WORKBENCH_LAYOUT_DATA_MAX_PRESENCE,
            &count, NULL) != UMI_STATUS_OK) {
        return 0U;
    }
    return count;
}
