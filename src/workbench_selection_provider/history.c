/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/history.c
 *
 * PURPOSE:
 *   Implement bounded dynamic provider history with oldest-record eviction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/history.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_capacity(
    UmiWorkbenchSelectionProviderHistory *history)
{
    UmiWorkbenchSelectionProviderHistoryItem *items;
    size_t next;
    if (history->count < history->capacity) return UMI_STATUS_OK;
    if (history->capacity >= UMI_WORKBENCH_SELECTION_PROVIDER_MAX_HISTORY) {
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
    if (next > UMI_WORKBENCH_SELECTION_PROVIDER_MAX_HISTORY) {
        next = UMI_WORKBENCH_SELECTION_PROVIDER_MAX_HISTORY;
    }
    items = (UmiWorkbenchSelectionProviderHistoryItem *)realloc(
        history->items,
        next * sizeof(history->items[0]));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
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

void umi_workbench_selection_provider_history_init(
    UmiWorkbenchSelectionProviderHistory *history)
{
    if (history == NULL) return;
    memset(history, 0, sizeof(*history));
    history->revision = 1U;
}

void umi_workbench_selection_provider_history_destroy(
    UmiWorkbenchSelectionProviderHistory *history)
{
    if (history == NULL) return;
    free(history->items);
    memset(history, 0, sizeof(*history));
}

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
    if (history == NULL || provider_id == NULL || selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = ensure_capacity(history);
    if (status != UMI_STATUS_OK) return status;
    item = &history->items[history->count++];
    memset(item, 0, sizeof(*item));
    status = umi_workbench_selection_provider_copy_text(
        item->provider_id, sizeof(item->provider_id), provider_id);
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

const UmiWorkbenchSelectionProviderHistoryItem *
umi_workbench_selection_provider_history_latest(
    const UmiWorkbenchSelectionProviderHistory *history,
    const char *provider_id)
{
    size_t index;
    if (history == NULL || provider_id == NULL) return NULL;
    index = history->count;
    while (index > 0U) {
        --index;
        if (strcmp(history->items[index].provider_id, provider_id) == 0) {
            return &history->items[index];
        }
    }
    return NULL;
}

void umi_workbench_selection_provider_history_clear(
    UmiWorkbenchSelectionProviderHistory *history)
{
    if (history == NULL) return;
    if (history->items != NULL && history->capacity > 0U) {
        memset(
            history->items,
            0,
            history->capacity * sizeof(history->items[0]));
    }
    history->count = 0U;
    ++history->revision;
}
