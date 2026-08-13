/*-----------------------------------------------------------------------------
 * Umicom Framework terminal view projection.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/terminal_view.h"
#include "view_internal.h"

#include <stdio.h>

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
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_terminal_ui_base_view(view_id, "terminal", "Integrated Terminal",
        "Framework-owned profiles, sessions, history, tabs and split layout.", out_view);
    if (status != UMI_STATUS_OK) return status;
    status = umi_terminal_controller_snapshot(controller, &snapshot);
    active = umi_terminal_controller_active_session(controller);
    if (status == UMI_STATUS_OK && active != NULL) {
        status = umi_terminal_session_snapshot(active, &active_snapshot);
    }
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.sessions", (int64_t)snapshot.sessions);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.profiles", (int64_t)snapshot.profiles);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.tabs", (int64_t)snapshot.tabs);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.split-nodes", (int64_t)snapshot.split_nodes);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_string(*out_view,
        "terminal.active-session", snapshot.active_session_id);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_string(*out_view,
        "terminal.working-directory", active_snapshot.working_directory);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.commands", (int64_t)active_snapshot.commands_executed);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.history", (int64_t)snapshot.history_entries);
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_integer(*out_view,
        "terminal.events", (int64_t)snapshot.events);
    for (index = 0U; status == UMI_STATUS_OK && index < snapshot.tabs; ++index) {
        UmiTerminalTabSnapshot tab;
        char key[96];
        if (umi_terminal_tab_model_at(umi_terminal_controller_tabs(controller),
                                      index, &tab) != UMI_STATUS_OK) continue;
        (void)snprintf(key, sizeof(key), "terminal.tab.%zu.id", index);
        status = umi_terminal_ui_set_string(*out_view, key, tab.session_id);
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "terminal.tab.%zu.title", index);
            status = umi_terminal_ui_set_string(*out_view, key, tab.title);
        }
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "terminal.tab.%zu.profile", index);
            status = umi_terminal_ui_set_string(*out_view, key, tab.profile_id);
        }
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "terminal.tab.%zu.active", index);
            status = umi_terminal_ui_set_boolean(*out_view, key, tab.active);
        }
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "terminal.tab.%zu.unread", index);
            status = umi_terminal_ui_set_integer(*out_view, key,
                                                  (int64_t)tab.unread_lines);
        }
    }
    if (status != UMI_STATUS_OK || active == NULL) return status;
    transcript = umi_terminal_session_transcript(active);
    count = umi_terminal_transcript_count(transcript);
    first = count > UMI_TERMINAL_UI_TRANSCRIPT_ROWS
        ? count - UMI_TERMINAL_UI_TRANSCRIPT_ROWS : 0U;
    status = umi_terminal_ui_set_integer(*out_view, "terminal.row-count",
                                         (int64_t)(count - first));
    for (index = first; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTerminalTranscriptLine line;
        char key[64];
        char value[UMI_TERMINAL_LINE_CAPACITY + 64U];
        if (umi_terminal_transcript_at(transcript, index, &line) != UMI_STATUS_OK) continue;
        (void)snprintf(key, sizeof(key), "terminal.row.%zu", index - first);
        (void)snprintf(value, sizeof(value), "[%s] %s",
                       umi_terminal_stream_text(line.stream), line.text);
        status = umi_terminal_ui_set_string(*out_view, key, value);
    }
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 0U,
        "studio.action.terminal.new", "New", "Open a new terminal profile");
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 1U,
        "studio.action.terminal.split", "Split", "Split the active terminal");
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 2U,
        "studio.action.terminal.execute", "Execute…", "Execute a trusted command");
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 3U,
        "studio.action.terminal.search", "Search", "Search retained transcript output");
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 4U,
        "studio.action.terminal.clear", "Clear", "Clear the active transcript");
    if (status == UMI_STATUS_OK) status = umi_terminal_ui_set_action(*out_view, 5U,
        "studio.action.terminal.close", "Close", "Close the active terminal");
    return status;
}
