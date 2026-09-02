/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal_ui/terminal_view.c
 *
 * PURPOSE:
 *   Implement the terminal view behavior for
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
 * Umicom Framework terminal view projection.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/terminal_view.h"
#include "view_internal.h"

#include <stdio.h>

/*
 * Initialise terminal ui terminal view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_terminal_ui_terminal_view_create(const char *view_id,
                                               UmiTerminalController *controller,
                                               UmiUiViewModel **out_view)
{
    UmiTerminalControllerSnapshot snapshot;
    UmiTerminalSessionSnapshot active_snapshot = {0};
    UmiTerminalSession *active;
    UmiTerminalTranscript *transcript;
    size_t count;
    size_t first;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_terminal_ui_base_view(view_id, "terminal", "Integrated Terminal",
        "Framework-owned profiles, sessions, history, tabs and split layout.", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_terminal_controller_snapshot(controller, &snapshot);
    active = umi_terminal_controller_active_session(controller);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && active != NULL) {
        status = umi_terminal_session_snapshot(active, &active_snapshot);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.sessions", (int64_t)snapshot.sessions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.profiles", (int64_t)snapshot.profiles);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.tabs", (int64_t)snapshot.tabs);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.split-nodes", (int64_t)snapshot.split_nodes);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_string(*out_view,
        "terminal.active-session", snapshot.active_session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_string(*out_view,
        "terminal.working-directory", active_snapshot.working_directory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.commands", (int64_t)active_snapshot.commands_executed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.history", (int64_t)snapshot.history_entries);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.events", (int64_t)snapshot.events);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < snapshot.tabs; ++index) {
        UmiTerminalTabSnapshot tab;
        char key[96];
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_terminal_tab_model_at(umi_terminal_controller_tabs(controller),
                                      index, &tab) != UMI_STATUS_OK) continue;
        (void)snprintf(key, sizeof(key), "terminal.tab.%zu.id", index);
        status = umi_terminal_ui_set_string(*out_view, key, tab.session_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "terminal.tab.%zu.title", index);
            status = umi_terminal_ui_set_string(*out_view, key, tab.title);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "terminal.tab.%zu.profile", index);
            status = umi_terminal_ui_set_string(*out_view, key, tab.profile_id);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "terminal.tab.%zu.active", index);
            status = umi_terminal_ui_set_boolean(*out_view, key, tab.active);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "terminal.tab.%zu.unread", index);
            status = umi_terminal_ui_set_integer(*out_view, key,
                                                  (int64_t)tab.unread_lines);
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || active == NULL) return status;
    transcript = umi_terminal_session_transcript(active);
    count = umi_terminal_transcript_count(transcript);
    first = count > UMI_TERMINAL_UI_TRANSCRIPT_ROWS
        ? count - UMI_TERMINAL_UI_TRANSCRIPT_ROWS : 0U;
    status = umi_terminal_ui_set_integer(*out_view, "terminal.row-count",
                                         (int64_t)(count - first));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = first; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTerminalTranscriptLine line;
        char key[64];
        char value[UMI_TERMINAL_LINE_CAPACITY + 64U];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_terminal_transcript_at(transcript, index, &line) != UMI_STATUS_OK) continue;
        (void)snprintf(key, sizeof(key), "terminal.row.%zu", index - first);
        (void)snprintf(value, sizeof(value), "[%s] %s",
                       umi_terminal_stream_text(line.stream), line.text);
        status = umi_terminal_ui_set_string(*out_view, key, value);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 0U,
        "studio.action.terminal.new", "New", "Open a new terminal profile");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 1U,
        "studio.action.terminal.split", "Split", "Split the active terminal");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 2U,
        "studio.action.terminal.execute", "Execute…", "Execute a trusted command");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 3U,
        "studio.action.terminal.search", "Search", "Search retained transcript output");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 4U,
        "studio.action.terminal.clear", "Clear", "Clear the active transcript");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 5U,
        "studio.action.terminal.close", "Close", "Close the active terminal");
    return status;
}
