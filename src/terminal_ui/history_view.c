/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal_ui/history_view.c
 *
 * PURPOSE:
 *   Implement the history view behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework terminal command-history view projection.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/history_view.h"
#include "view_internal.h"

#include <stdio.h>

/*
 * Initialise terminal ui history view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_terminal_ui_history_view_create(const char *view_id,
                                              UmiTerminalController *controller,
                                              UmiUiViewModel **out_view)
{
    UmiTerminalHistory *history;
    UmiTerminalHistoryStats stats;
    size_t first;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    history = umi_terminal_controller_history(controller);
    stats = umi_terminal_history_stats(history);
    status = umi_terminal_ui_base_view(view_id, "terminal-history", "Terminal History",
        "Retained commands with session, working directory and exit status.", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "history.count", (int64_t)stats.count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "history.succeeded", (int64_t)stats.succeeded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "history.failed", (int64_t)stats.failed);
    first = stats.count > UMI_TERMINAL_UI_TRANSCRIPT_ROWS
        ? stats.count - UMI_TERMINAL_UI_TRANSCRIPT_ROWS : 0U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "history.row-count", (int64_t)(stats.count - first));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = first; status == UMI_STATUS_OK && index < stats.count; ++index) {
        UmiTerminalHistoryEntry entry;
        char key[64];
        char value[UMI_TERMINAL_COMMAND_CAPACITY + 192U];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_terminal_history_at(history, index, &entry) != UMI_STATUS_OK) continue;
        (void)snprintf(key, sizeof(key), "history.row.%zu", index - first);
        (void)snprintf(value, sizeof(value), "[%s] exit=%d  %s",
                       entry.session_id, entry.exit_code, entry.command);
        status = umi_terminal_ui_set_string(*out_view, key, value);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 0U,
        "studio.action.terminal.history-clear", "Clear History",
        "Clear retained terminal commands");
    return status;
}
