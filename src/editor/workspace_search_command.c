/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_search_command.c
 *
 * PURPOSE:
 *   Implement the canonical workspace-search command catalogue. The ordering is
 *   stable and intentionally groups search, policy, result and replacement work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_search_command.h"

#include <string.h>

#define COMMAND(kind_value, command_id, command_label, keybinding, index_req, \
                results_req, replacement_req, confirmation_req, mutates)      \
    {                                                                          \
        (uint32_t)sizeof(UmiEditorWorkspaceSearchCommandDescriptor),           \
        UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_API_VERSION,                        \
        kind_value, command_id, command_label, keybinding, index_req,           \
        results_req, replacement_req, confirmation_req, mutates                 \
    }

static const UmiEditorWorkspaceSearchCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_OPEN,
            "search.workspace.open", "Open Workspace Search", "Ctrl+Shift+F",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_EXECUTE,
            "search.workspace.execute", "Search Workspace", "Enter",
            1, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_REFRESH_INDEX,
            "search.workspace.refresh-index", "Refresh Search Index", "",
            1, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_NEXT_RESULT,
            "search.workspace.next-result", "Next Search Result", "F4",
            0, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_PREVIOUS_RESULT,
            "search.workspace.previous-result", "Previous Search Result",
            "Shift+F4", 0, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_CASE,
            "search.workspace.toggle-case", "Match Case", "Alt+C",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_REGEX,
            "search.workspace.toggle-regex", "Use Regular Expression", "Alt+R",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_WHOLE_WORD,
            "search.workspace.toggle-whole-word", "Match Whole Word", "Alt+W",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_MULTILINE,
            "search.workspace.toggle-multiline", "Multiline Search", "Alt+M",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_TOGGLE_OVERLAP,
            "search.workspace.toggle-overlap", "Allow Overlapping Matches", "",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_ADD_INCLUDE,
            "search.workspace.add-include", "Add Search Include Rule", "",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_ADD_EXCLUDE,
            "search.workspace.add-exclude", "Add Search Exclude Rule", "",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_CLEAR_RESULTS,
            "search.workspace.clear-results", "Clear Search Results", "Escape",
            0, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_COPY_RESULTS,
            "search.workspace.copy-results", "Copy Search Results", "",
            0, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_EXPORT_RESULTS,
            "search.workspace.export-results", "Export Search Results", "",
            0, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_OPEN_REPLACE,
            "replace.workspace.open", "Open Workspace Replace", "Ctrl+Shift+H",
            0, 0, 1, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_BUILD_PREVIEW,
            "replace.workspace.build-preview", "Preview Workspace Replacement",
            "Ctrl+Enter", 1, 1, 1, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_SELECT_ALL,
            "replace.workspace.select-all", "Select All Replacements", "",
            0, 1, 1, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_CLEAR_SELECTION,
            "replace.workspace.clear-selection", "Clear Replacement Selection",
            "", 0, 1, 1, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_PREPARE_REPLACE,
            "replace.workspace.prepare", "Prepare Workspace Replacement", "",
            1, 1, 1, 0, 0),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_APPLY_REPLACE,
            "replace.workspace.apply", "Apply Workspace Replacement", "",
            1, 1, 1, 1, 1),
    COMMAND(UMI_EDITOR_WORKSPACE_SEARCH_COMMAND_CANCEL_REPLACE,
            "replace.workspace.cancel", "Cancel Workspace Replacement", "Escape",
            0, 0, 1, 0, 0)
};

#undef COMMAND

size_t umi_editor_workspace_search_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiEditorWorkspaceSearchCommandDescriptor *
umi_editor_workspace_search_command_at(size_t position)
{
    return position < umi_editor_workspace_search_command_count()
        ? &COMMANDS[position] : NULL;
}

const UmiEditorWorkspaceSearchCommandDescriptor *
umi_editor_workspace_search_command_find(const char *command_id)
{
    size_t position;
    if (command_id == NULL || command_id[0] == '\0') return NULL;
    for (position = 0U;
         position < umi_editor_workspace_search_command_count();
         ++position) {
        if (strcmp(COMMANDS[position].id, command_id) == 0) {
            return &COMMANDS[position];
        }
    }
    return NULL;
}

const UmiEditorWorkspaceSearchCommandDescriptor *
umi_editor_workspace_search_command_for_kind(
    UmiEditorWorkspaceSearchCommandKind kind)
{
    size_t position;
    for (position = 0U;
         position < umi_editor_workspace_search_command_count();
         ++position) {
        if (COMMANDS[position].kind == kind) return &COMMANDS[position];
    }
    return NULL;
}
