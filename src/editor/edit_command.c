/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/edit_command.c
 *
 * PURPOSE:
 *   Publish stable identifiers and defaults for Framework-owned professional
 *   editing commands. Products decide menu placement but do not redefine the
 *   command semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/edit_command.h"

#include <string.h>

#define COMMAND(kind_value, id_value, label_value, key_value, selection, line) \
    {                                                                            \
        (uint32_t)sizeof(UmiEditorEditCommandDescriptor),                        \
        UMI_EDITOR_EDIT_COMMAND_API_VERSION,                                     \
        kind_value, id_value, label_value, key_value, 1, selection, line         \
    }

static const UmiEditorEditCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT,
            "editor.edit.insert-text", "Insert Text", "", 0, 0),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_REPLACE_RANGE,
            "editor.edit.replace-range", "Replace Range", "", 1, 0),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_DELETE_LINE,
            "editor.edit.delete-line", "Delete Line", "Ctrl+Shift+K", 0, 1),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE,
            "editor.edit.duplicate-line", "Duplicate Line",
            "Ctrl+Shift+D", 0, 1),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP,
            "editor.edit.move-line-up", "Move Line Up", "Alt+Up", 0, 1),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN,
            "editor.edit.move-line-down", "Move Line Down", "Alt+Down", 0, 1),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT,
            "editor.edit.join-line-next", "Join Line with Next",
            "Ctrl+J", 0, 1),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE,
            "editor.edit.trim-trailing-whitespace", "Trim Trailing Whitespace",
            "", 0, 1),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_INDENT_LINES,
            "editor.edit.indent-lines", "Indent Lines", "Tab", 1, 1),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES,
            "editor.edit.outdent-lines", "Outdent Lines", "Shift+Tab", 1, 1),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT,
            "editor.edit.toggle-line-comment", "Toggle Line Comment",
            "Ctrl+/", 1, 1),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE,
            "editor.edit.transform-uppercase", "Transform to Uppercase",
            "", 1, 0),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE,
            "editor.edit.transform-lowercase", "Transform to Lowercase",
            "", 1, 0),
    COMMAND(UMI_EDITOR_EDIT_COMMAND_TRANSPOSE_LINES,
            "editor.edit.transpose-lines", "Transpose Line with Next",
            "", 0, 1)
};

#undef COMMAND

size_t umi_editor_edit_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiEditorEditCommandDescriptor *umi_editor_edit_command_at(size_t index)
{
    return index < umi_editor_edit_command_count() ? &COMMANDS[index] : NULL;
}

const UmiEditorEditCommandDescriptor *umi_editor_edit_command_find(
    const char *id)
{
    size_t index;

    if (id == NULL) return NULL;
    for (index = 0U; index < umi_editor_edit_command_count(); ++index) {
        if (strcmp(COMMANDS[index].id, id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

const UmiEditorEditCommandDescriptor *umi_editor_edit_command_for_kind(
    UmiEditorEditCommandKind kind)
{
    size_t index;

    for (index = 0U; index < umi_editor_edit_command_count(); ++index) {
        if (COMMANDS[index].kind == kind) return &COMMANDS[index];
    }
    return NULL;
}

UmiStatus umi_editor_edit_command_request_initialize(
    UmiEditorEditCommandRequest *request,
    UmiEditorEditCommandKind kind,
    size_t cursor_offset)
{
    if (request == NULL || umi_editor_edit_command_for_kind(kind) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_EDITOR_EDIT_COMMAND_API_VERSION;
    request->kind = kind;
    request->cursor_offset = cursor_offset;
    request->selection_start = cursor_offset;
    request->selection_end = cursor_offset;
    return UMI_STATUS_OK;
}
