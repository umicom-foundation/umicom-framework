/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/offline_queue.c
 *
 * PURPOSE:
 *   Persist offline layout operations for deterministic replay after connectivity returns.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/offline_queue.h"
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
    return umi_workbench_layout_offline_queue_encode(
        (const UmiWorkbenchLayoutOfflineOperation *)record, buffer, capacity, out_required);
}

/* Provide the decode bridge operation used by this module and its client applications. */
static UmiStatus decode_bridge(
    const char *value,
    void *out_record)
{
    return umi_workbench_layout_offline_queue_decode(
        value, (UmiWorkbenchLayoutOfflineOperation *)out_record);
}

/*
 * Write workbench layout offline queue in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_offline_queue_encode(
    const UmiWorkbenchLayoutOfflineOperation *record,
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
            &fields, "operation_id", record->operation_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "layout_id", record->layout_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "actor_id", record->actor_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "correlation_id", record->correlation_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "change_kind", (uint32_t)record->change_kind);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "state", (uint32_t)record->state);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "expected_revision", record->expected_revision);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "created_at_ms", record->created_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "available_at_ms", record->available_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u64(
            &fields, "claimed_until_ms", record->claimed_until_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "attempt_count", record->attempt_count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put_u32(
            &fields, "maximum_attempts", record->maximum_attempts);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_put(
            &fields, "payload_reference", record->payload_reference);
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
 * Read workbench layout offline queue into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_offline_queue_decode(
    const char *value,
    UmiWorkbenchLayoutOfflineOperation *out_record)
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
            &fields, "operation_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->operation_id, sizeof(out_record->operation_id), text, true)
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
            &fields, "actor_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->actor_id, sizeof(out_record->actor_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "correlation_id");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->correlation_id, sizeof(out_record->correlation_id), text, true)
            : UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        uint32_t parsed = 0U;
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "change_kind", &parsed);
        out_record->change_kind = (UmiWorkbenchLayoutDataChangeKind)parsed;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        uint32_t parsed = 0U;
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "state", &parsed);
        out_record->state = (UmiWorkbenchLayoutDataOperationState)parsed;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "expected_revision", &out_record->expected_revision);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "created_at_ms", &out_record->created_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "available_at_ms", &out_record->available_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "claimed_until_ms", &out_record->claimed_until_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "attempt_count", &out_record->attempt_count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u32(
            &fields, "maximum_attempts", &out_record->maximum_attempts);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        const char *text = umi_workbench_layout_data_field_set_get(
            &fields, "payload_reference");
        status = text != NULL
            ? umi_workbench_layout_data_copy_text(
                out_record->payload_reference, sizeof(out_record->payload_reference), text, true)
            : UMI_STATUS_NOT_FOUND;
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
 * Initialise workbench layout offline queue repository from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_workbench_layout_offline_queue_repository_init(
    UmiWorkbenchLayoutOfflineQueueRepository *repository,
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
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_OFFLINE_OPERATION,
        sizeof(UmiWorkbenchLayoutOfflineOperation),
        encode_bridge,
        decode_bridge);
    return status;
}

/*
 * Write workbench layout offline queue in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_offline_queue_save(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    const UmiWorkbenchLayoutOfflineOperation *record)
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
        record->operation_id,
        0U,
        record);
}

/*
 * Read workbench layout offline queue into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_offline_queue_load(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutOfflineOperation *out_record)
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
 * Provide the workbench layout offline queue delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_delete(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
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
 * Provide the workbench layout offline queue list operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_list(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutOfflineOperation *records,
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

typedef struct AvailablePredicateContext {
    uint64_t now_ms;
} AvailablePredicateContext;

/*
 * Provide the available predicate operation used by this module and its client
 * applications.
 */
static UmiStatus available_predicate(
    const void *record,
    void *context,
    bool *out_matches)
{
    const UmiWorkbenchLayoutOfflineOperation *operation = (const UmiWorkbenchLayoutOfflineOperation *)record;
    const AvailablePredicateContext *criteria =
        (const AvailablePredicateContext *)context;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL || criteria == NULL || out_matches == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matches =
        (operation->state == UMI_WORKBENCH_LAYOUT_DATA_OPERATION_PENDING ||
         operation->state == UMI_WORKBENCH_LAYOUT_DATA_OPERATION_RETRY_WAIT) &&
        operation->available_at_ms <= criteria->now_ms;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout offline queue claim available operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_claim_available(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    uint64_t now_ms,
    uint64_t lease_duration_ms,
    UmiWorkbenchLayoutOfflineOperation *out_operation)
{
    UmiWorkbenchLayoutOfflineOperation available[32];
    AvailablePredicateContext criteria;
    size_t count = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || out_operation == NULL ||
        lease_duration_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    criteria.now_ms = now_ms;
    status = umi_workbench_layout_offline_queue_list(
        repository, NULL, available_predicate, &criteria,
        available, 32U, &count, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
    *out_operation = available[0];
    out_operation->state = UMI_WORKBENCH_LAYOUT_DATA_OPERATION_CLAIMED;
    out_operation->claimed_until_ms = now_ms + lease_duration_ms;
    out_operation->attempt_count += 1U;
    return umi_workbench_layout_offline_queue_save(repository, out_operation);
}

/*
 * Provide the workbench layout offline queue complete operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_complete(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    UmiWorkbenchLayoutOfflineOperation *operation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operation->state = UMI_WORKBENCH_LAYOUT_DATA_OPERATION_COMPLETED;
    operation->claimed_until_ms = 0U;
    return umi_workbench_layout_offline_queue_save(repository, operation);
}

/*
 * Provide the workbench layout offline queue retry operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_retry(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    UmiWorkbenchLayoutOfflineOperation *operation,
    uint64_t now_ms,
    uint64_t retry_delay_ms,
    const char *error)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || operation == NULL || error == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operation->claimed_until_ms = 0U;
    (void)umi_workbench_layout_data_copy_text(
        operation->last_error, sizeof(operation->last_error),
        error, true);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (operation->attempt_count >= operation->maximum_attempts) {
        operation->state = UMI_WORKBENCH_LAYOUT_DATA_OPERATION_FAILED;
        operation->available_at_ms = 0U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        operation->state = UMI_WORKBENCH_LAYOUT_DATA_OPERATION_RETRY_WAIT;
        operation->available_at_ms = now_ms + retry_delay_ms;
    }
    return umi_workbench_layout_offline_queue_save(repository, operation);
}
