/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/history.c
 *
 * PURPOSE:
 *   Implement bounded dynamic provider history with oldest-record eviction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/history.h"

#include <stdlib.h>
#include <string.h>

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(
    UmiWorkbenchSelectionProviderHistory *history)
{
    UmiWorkbenchSelectionProviderHistoryItem *items;
    size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count < history->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->capacity >= UMI_WORKBENCH_SELECTION_PROVIDER_MAX_HISTORY) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (history->count > 0U) {
            memmove(
                &history->items[0],
                &history->items[1],
                (history->count - 1U) * sizeof(history->items[0]));
            --history->count;
        }
        return UMI_STATUS_OK;
    }
    next = history->capacity == 0U ? 32U : history->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_WORKBENCH_SELECTION_PROVIDER_MAX_HISTORY) {
        next = UMI_WORKBENCH_SELECTION_PROVIDER_MAX_HISTORY;
    }
    items = (UmiWorkbenchSelectionProviderHistoryItem *)realloc(
        history->items,
        next * sizeof(history->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (next > history->capacity) {
        memset(
            items + history->capacity,
            0,
            (next - history->capacity) * sizeof(items[0]));
    }
    history->items = items;
    history->capacity = next;
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench selection provider history from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_history_init(
    UmiWorkbenchSelectionProviderHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return;
    memset(history, 0, sizeof(*history));
    history->revision = 1U;
}

/*
 * Release or reset state held by workbench selection provider history so the same storage
 * can be reused safely.
 */
void umi_workbench_selection_provider_history_destroy(
    UmiWorkbenchSelectionProviderHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return;
    free(history->items);
    memset(history, 0, sizeof(*history));
}

/*
 * Add workbench selection provider history only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_workbench_selection_provider_history_append(
    UmiWorkbenchSelectionProviderHistory *history,
    const char *provider_id,
    const UmiWorkbenchSelection *selection,
    UmiWorkbenchSelectionProviderResult result,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    UmiWorkbenchSelectionProviderHistoryItem *item;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || provider_id == NULL || selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = ensure_capacity(history);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    item = &history->items[history->count++];
    memset(item, 0, sizeof(*item));
    status = umi_workbench_selection_provider_copy_text(
        item->provider_id, sizeof(item->provider_id), provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        --history->count;
        return status;
    }
    item->selection = *selection;
    item->result = result;
    item->sequence = sequence;
    item->timestamp_ms = timestamp_ms;
    ++history->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection provider history latest operation used by this module
 * and its client applications.
 */
const UmiWorkbenchSelectionProviderHistoryItem *
umi_workbench_selection_provider_history_latest(
    const UmiWorkbenchSelectionProviderHistory *history,
    const char *provider_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || provider_id == NULL) return NULL;
    index = history->count;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        --index;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(history->items[index].provider_id, provider_id) == 0) {
            return &history->items[index];
        }
    }
    return NULL;
}

/*
 * Release or reset state held by workbench selection provider history so the same storage
 * can be reused safely.
 */
void umi_workbench_selection_provider_history_clear(
    UmiWorkbenchSelectionProviderHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history->items != NULL && history->capacity > 0U) {
        memset(
            history->items,
            0,
            history->capacity * sizeof(history->items[0]));
    }
    history->count = 0U;
    ++history->revision;
}
