/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/productivity_command.c
 *
 * PURPOSE:
 *   Implement the canonical editing-productivity command catalogue consumed
 *   by Studio and future lightweight Umicom application shells.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/productivity_command.h"

#include <string.h>

#define COMMAND(command_kind, command_id, command_label, key, document, selection, active, view) \
    {                                                                                           \
        (uint32_t)sizeof(UmiEditorProductivityCommandDescriptor),                               \
        UMI_EDITOR_PRODUCTIVITY_COMMAND_API_VERSION, command_kind, command_id,                   \
        command_label, key, document, selection, active, view                                   \
    }

static const UmiEditorProductivityCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_FORMAT_DOCUMENT,
            "editor.productivity.format.document", "Format Document",
            "Shift+Alt+F", 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_FORMAT_SELECTION,
            "editor.productivity.format.selection", "Format Selection",
            "Ctrl+K, Ctrl+F", 1, 1, 0, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_TOGGLE_FORMAT_ON_SAVE,
            "editor.productivity.format.on-save", "Toggle Format on Save",
            "", 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_TOGGLE_FORMAT_ON_TYPE,
            "editor.productivity.format.on-type", "Toggle Format on Type",
            "", 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_SELECT_FORMATTER,
            "editor.productivity.format.select-provider", "Select Formatter",
            "", 1, 0, 0, 1),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_ORGANISE_IMPORTS,
            "editor.productivity.imports.organise", "Organise Imports",
            "Shift+Alt+O", 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_REMOVE_UNUSED_IMPORTS,
            "editor.productivity.imports.remove-unused",
            "Remove Unused Imports", "", 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_SORT_IMPORTS,
            "editor.productivity.imports.sort", "Sort Imports",
            "", 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_INSERT_SNIPPET,
            "editor.productivity.snippet.insert", "Insert Snippet",
            "Ctrl+Space", 1, 0, 0, 1),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_NEXT_SNIPPET_PLACEHOLDER,
            "editor.productivity.snippet.next-placeholder",
            "Next Snippet Placeholder", "Tab", 1, 0, 1, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_PREVIOUS_SNIPPET_PLACEHOLDER,
            "editor.productivity.snippet.previous-placeholder",
            "Previous Snippet Placeholder", "Shift+Tab", 1, 0, 1, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_CANCEL_SNIPPET,
            "editor.productivity.snippet.cancel", "Cancel Snippet Session",
            "Escape", 0, 0, 1, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_START_LINKED_EDITING,
            "editor.productivity.linked.start", "Start Linked Editing",
            "", 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_NEXT_LINKED_RANGE,
            "editor.productivity.linked.next-range", "Next Linked Range",
            "Tab", 1, 0, 1, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_APPLY_LINKED_EDIT,
            "editor.productivity.linked.apply", "Apply Linked Edit",
            "Enter", 1, 0, 1, 0),
    COMMAND(UMI_EDITOR_PRODUCTIVITY_COMMAND_CANCEL_LINKED_EDITING,
            "editor.productivity.linked.cancel", "Cancel Linked Editing",
            "Escape", 0, 0, 1, 0)
};

#undef COMMAND

size_t umi_editor_productivity_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiEditorProductivityCommandDescriptor *
umi_editor_productivity_command_at(size_t index)
{
    return index < umi_editor_productivity_command_count() ? &COMMANDS[index]
                                                            : NULL;
}

const UmiEditorProductivityCommandDescriptor *
umi_editor_productivity_command_find(const char *id)
{
    size_t index;

    if (id == NULL) return NULL;
    for (index = 0U; index < umi_editor_productivity_command_count(); ++index) {
        if (strcmp(COMMANDS[index].id, id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

const UmiEditorProductivityCommandDescriptor *
umi_editor_productivity_command_for_kind(UmiEditorProductivityCommandKind kind)
{
    size_t index;

    for (index = 0U; index < umi_editor_productivity_command_count(); ++index) {
        if (COMMANDS[index].kind == kind) return &COMMANDS[index];
    }
    return NULL;
}
