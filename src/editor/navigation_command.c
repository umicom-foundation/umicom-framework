/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/navigation_command.c
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned source-navigation command catalogue
 *   shared by Studio and future Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/navigation_command.h"

#include <string.h>

#define COMMAND(command_kind, command_id, command_label, key, query, required, multiple) \
    {                                                                                     \
        (uint32_t)sizeof(UmiEditorNavigationCommandDescriptor),                           \
        UMI_EDITOR_NAVIGATION_COMMAND_API_VERSION, command_kind, command_id,              \
        command_label, key, query, required, multiple                                     \
    }

static const UmiEditorNavigationCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_GO_TO_DEFINITION,
            "editor.navigate.definition", "Go to Definition", "F12",
            UMI_EDITOR_NAVIGATION_QUERY_DEFINITION, 1, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_PEEK_DEFINITION,
            "editor.navigate.peek-definition", "Peek Definition", "Alt+F12",
            UMI_EDITOR_NAVIGATION_QUERY_DEFINITION, 1, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_GO_TO_DECLARATION,
            "editor.navigate.declaration", "Go to Declaration", "",
            UMI_EDITOR_NAVIGATION_QUERY_DECLARATION, 1, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_GO_TO_TYPE_DEFINITION,
            "editor.navigate.type-definition", "Go to Type Definition", "",
            UMI_EDITOR_NAVIGATION_QUERY_TYPE_DEFINITION, 1, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_GO_TO_IMPLEMENTATION,
            "editor.navigate.implementation", "Go to Implementation", "Ctrl+F12",
            UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION, 1, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_FIND_REFERENCES,
            "editor.navigate.references", "Find All References", "Shift+F12",
            UMI_EDITOR_NAVIGATION_QUERY_REFERENCE, 1, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_GO_BACK,
            "editor.navigate.back", "Go Back", "Alt+Left",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_GO_FORWARD,
            "editor.navigate.forward", "Go Forward", "Alt+Right",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_DOCUMENT_SYMBOLS,
            "editor.navigate.document-symbols", "Go to Symbol in Editor",
            "Ctrl+Shift+O", UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL, 0, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_WORKSPACE_SYMBOLS,
            "editor.navigate.workspace-symbols", "Go to Symbol in Workspace",
            "Ctrl+T", UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL, 0, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_FOCUS_BREADCRUMBS,
            "editor.navigate.focus-breadcrumbs", "Focus Breadcrumbs", "Ctrl+Shift+.",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_TOGGLE_OUTLINE,
            "editor.navigate.toggle-outline", "Toggle Outline", "",
            UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL, 0, 0)
};

#undef COMMAND

size_t umi_editor_navigation_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiEditorNavigationCommandDescriptor *
umi_editor_navigation_command_at(size_t index)
{
    return index < umi_editor_navigation_command_count() ? &COMMANDS[index]
                                                         : NULL;
}

const UmiEditorNavigationCommandDescriptor *
umi_editor_navigation_command_find(const char *id)
{
    size_t index;
    if (id == NULL) return NULL;
    for (index = 0U; index < umi_editor_navigation_command_count(); ++index) {
        if (strcmp(COMMANDS[index].id, id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

const UmiEditorNavigationCommandDescriptor *
umi_editor_navigation_command_for_kind(UmiEditorNavigationCommandKind kind)
{
    size_t index;
    for (index = 0U; index < umi_editor_navigation_command_count(); ++index) {
        if (COMMANDS[index].kind == kind) return &COMMANDS[index];
    }
    return NULL;
}
