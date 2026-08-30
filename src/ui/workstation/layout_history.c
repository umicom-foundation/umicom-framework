/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_history.c
 *
 * PURPOSE:
 *   Implement bounded layout history navigation for compare, undo-like recovery and visual layout browser timelines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/layout_history.h"

void umi_ws_layout_history_init(UmiWsLayoutHistory *history) {
    if (history != NULL) *history = (UmiWsLayoutHistory){0};
}

UmiStatus umi_ws_layout_history_push(UmiWsLayoutHistory *history, const UmiWsLayoutSnapshot *snapshot) {
    size_t index;
    if (history == NULL || snapshot == NULL || !umi_ws_id_valid(snapshot->snapshot_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->cursor + 1U < history->count) history->count = history->cursor + 1U;
    if (history->count >= UMI_WS_LAYOUT_HISTORY_MAX) {
        for (index = 1U; index < history->count; ++index) history->entries[index - 1U] = history->entries[index];
        history->count = UMI_WS_LAYOUT_HISTORY_MAX - 1U;
        if (history->cursor > 0U) --history->cursor;
    }
    history->entries[history->count++] = *snapshot;
    history->cursor = history->count - 1U;
    return UMI_STATUS_OK;
}

const UmiWsLayoutSnapshot *umi_ws_layout_history_back(UmiWsLayoutHistory *history) {
    if (history == NULL || history->count == 0U) return NULL;
    if (history->cursor > 0U) --history->cursor;
    return &history->entries[history->cursor];
}

const UmiWsLayoutSnapshot *umi_ws_layout_history_forward(UmiWsLayoutHistory *history) {
    if (history == NULL || history->count == 0U) return NULL;
    if (history->cursor + 1U < history->count) ++history->cursor;
    return &history->entries[history->cursor];
}

const UmiWsLayoutSnapshot *umi_ws_layout_history_current(const UmiWsLayoutHistory *history) {
    if (history == NULL || history->count == 0U || history->cursor >= history->count) return NULL;
    return &history->entries[history->cursor];
}
