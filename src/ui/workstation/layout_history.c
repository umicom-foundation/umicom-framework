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

/*
 * Initialise ws layout history from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_layout_history_init(UmiWsLayoutHistory *history) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history != NULL) *history = (UmiWsLayoutHistory){0};
}

/*
 * Provide the ws layout history push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_history_push(UmiWsLayoutHistory *history, const UmiWsLayoutSnapshot *snapshot) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || snapshot == NULL || !umi_ws_id_valid(snapshot->snapshot_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->cursor + 1U < history->count) history->count = history->cursor + 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count >= UMI_WS_LAYOUT_HISTORY_MAX) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 1U; index < history->count; ++index) history->entries[index - 1U] = history->entries[index];
        history->count = UMI_WS_LAYOUT_HISTORY_MAX - 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (history->cursor > 0U) --history->cursor;
    }
    history->entries[history->count++] = *snapshot;
    history->cursor = history->count - 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws layout history back operation used by this module and its client
 * applications.
 */
const UmiWsLayoutSnapshot *umi_ws_layout_history_back(UmiWsLayoutHistory *history) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || history->count == 0U) return NULL;
    /* Apply this branch only when its contract condition is satisfied. */
    if (history->cursor > 0U) --history->cursor;
    return &history->entries[history->cursor];
}

/*
 * Provide the ws layout history forward operation used by this module and its client
 * applications.
 */
const UmiWsLayoutSnapshot *umi_ws_layout_history_forward(UmiWsLayoutHistory *history) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || history->count == 0U) return NULL;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->cursor + 1U < history->count) ++history->cursor;
    return &history->entries[history->cursor];
}

/*
 * Provide the ws layout history current operation used by this module and its client
 * applications.
 */
const UmiWsLayoutSnapshot *umi_ws_layout_history_current(const UmiWsLayoutHistory *history) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || history->count == 0U || history->cursor >= history->count) return NULL;
    return &history->entries[history->cursor];
}
