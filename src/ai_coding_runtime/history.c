/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/history.c
 *
 * PURPOSE:
 *   Implement bounded oldest-first agent snapshot retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/history.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingHistory {
    UmiAiCodingAgentSnapshot
        items[UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY];
    size_t count;
};

/*
 * Initialise ai coding history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_history_create(UmiAiCodingHistory **out_history)
{
    UmiAiCodingHistory *history;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;

    history = (UmiAiCodingHistory *)calloc(1U, sizeof(*history));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_history = history;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding history so the same storage can be reused
 * safely.
 */
void umi_ai_coding_history_destroy(UmiAiCodingHistory *history)
{
    free(history);
}

/*
 * Provide the ai coding history record operation used by this module and its client
 * applications.
 */
/* Validate incoming observation text before eviction and reuse one atomic retention operation for single records and groups. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_history_record(
    UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count == UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY) {
        (void)memmove(
            &history->items[0],
            &history->items[1],
            (history->count - 1U) * sizeof(history->items[0]));
        history->count -= 1U;
    }

    history->items[history->count++] = *snapshot;
    return UMI_STATUS_OK;
}

#endif
static UmiStatus history_check(const UmiAiCodingHistory *history)
{
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return history->count <= UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* Validate the whole bounded group before evicting older evidence. At most
 * one retention shift is needed, preserving the newest records in order. */
/* Share read-only admission with composed observation publication so all owners can be checked before any retention changes. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiAiCodingHistoryRecordBatch(UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshots, size_t snapshot_count)
{
    UmiStatus status = history_check(history);
    if (status != UMI_STATUS_OK) return status;
    if (snapshots == NULL && snapshot_count != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (snapshot_count > UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < snapshot_count; ++i) {
        status = UmiAiCodingAgentSnapshotValidateText(&snapshots[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    if (snapshot_count == 0U) return UMI_STATUS_OK;
    size_t room = UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY - history->count;
    size_t dropped = snapshot_count > room ? snapshot_count - room : 0U;
    size_t retained = history->count - dropped;
    if (dropped != 0U && retained != 0U)
        memmove(history->items, &history->items[dropped], retained * sizeof history->items[0]);
    memcpy(&history->items[retained], snapshots, snapshot_count * sizeof history->items[0]);
    history->count = retained + snapshot_count;
    return UMI_STATUS_OK;
}

#endif
/* Read-only admission shares exactly the checks used by the mutator.
 * Serialize the owner until the matching operation completes; this is not a reservation. */
UmiStatus UmiAiCodingHistoryCheckRecordBatch(const UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshots, size_t snapshot_count)
{
    UmiStatus status = history_check(history);
    if (status != UMI_STATUS_OK) return status;
    if (snapshots == NULL && snapshot_count != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (snapshot_count > UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < snapshot_count; ++i) {
        status = UmiAiCodingAgentSnapshotValidateText(&snapshots[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiCodingHistoryRecordBatch(UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshots, size_t snapshot_count)
{
    UmiStatus status = UmiAiCodingHistoryCheckRecordBatch(history, snapshots, snapshot_count);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot_count == 0U) return UMI_STATUS_OK;
    size_t room = UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY - history->count;
    size_t dropped = snapshot_count > room ? snapshot_count - room : 0U;
    size_t retained = history->count - dropped;
    if (dropped != 0U && retained != 0U)
        memmove(history->items, &history->items[dropped], retained * sizeof history->items[0]);
    memcpy(&history->items[retained], snapshots, snapshot_count * sizeof history->items[0]);
    history->count = retained + snapshot_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_history_record(
    UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshot)
{
    return UmiAiCodingHistoryRecordBatch(history, snapshot, 1U);
}

/*
 * Find ai coding history while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Expose bounded history pages and validate retained text before publishing any output. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_history_at(
    const UmiAiCodingHistory *history,
    size_t index,
    UmiAiCodingAgentSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= history->count) return UMI_STATUS_NOT_FOUND;

    *out_snapshot = history->items[index];
    return UMI_STATUS_OK;
}

#endif
UmiStatus UmiAiCodingHistoryCopyRange(const UmiAiCodingHistory *history,
    size_t offset, UmiAiCodingAgentSnapshot *snapshots, size_t capacity,
    size_t *out_count)
{
    UmiStatus status = history_check(history);
    if (status != UMI_STATUS_OK) return status;
    if (out_count == NULL || (snapshots == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (offset > history->count) return UMI_STATUS_NOT_FOUND;
    size_t available = history->count - offset;
    size_t count = available < capacity ? available : capacity;
    for (size_t i = 0U; i < count; ++i) {
        status = UmiAiCodingAgentSnapshotValidateText(&history->items[offset + i]);
        if (status != UMI_STATUS_OK) return status;
    }
    if (count != 0U)
        memcpy(snapshots, &history->items[offset], count * sizeof history->items[0]);
    *out_count = count;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_history_at(
    const UmiAiCodingHistory *history,
    size_t index,
    UmiAiCodingAgentSnapshot *out_snapshot)
{
    size_t copied = 0U;
    UmiStatus status = UmiAiCodingHistoryCopyRange(history, index, out_snapshot, 1U, &copied);
    return status != UMI_STATUS_OK ? status :
        copied == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai coding history without changing their
 * state.
 */
size_t umi_ai_coding_history_count(const UmiAiCodingHistory *history)
{
    return history != NULL ? history->count : 0U;
}
