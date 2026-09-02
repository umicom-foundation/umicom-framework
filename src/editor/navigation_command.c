/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/navigation_command.c
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned source-navigation command catalogue
 *   shared by Studio and future Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
            UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_SHOW_CALL_HIERARCHY,
            "editor.navigate.call-hierarchy", "Show Call Hierarchy", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 1, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_SHOW_INCOMING_CALLS,
            "editor.navigate.incoming-calls", "Show Incoming Calls", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 1, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_SHOW_OUTGOING_CALLS,
            "editor.navigate.outgoing-calls", "Show Outgoing Calls", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 1, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_SHOW_TYPE_HIERARCHY,
            "editor.navigate.type-hierarchy", "Show Type Hierarchy", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 1, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_SHOW_SUPERTYPES,
            "editor.navigate.supertypes", "Show Supertypes", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 1, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_SHOW_SUBTYPES,
            "editor.navigate.subtypes", "Show Subtypes", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 1, 1),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_PEEK_NEXT_RESULT,
            "editor.navigate.peek-next", "Next Peek Result", "Alt+Down",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_PEEK_PREVIOUS_RESULT,
            "editor.navigate.peek-previous", "Previous Peek Result", "Alt+Up",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_PEEK_PIN,
            "editor.navigate.peek-pin", "Pin Peek Definition", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_PEEK_CLOSE,
            "editor.navigate.peek-close", "Close Peek Definition", "Escape",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_REFRESH,
            "editor.navigate.refresh", "Refresh Navigation Results", "F5",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_CANCEL,
            "editor.navigate.cancel", "Cancel Navigation Request", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_OPEN_TO_SIDE,
            "editor.navigate.open-to-side", "Open Navigation Result to Side",
            "Ctrl+Enter", UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_OPEN_IN_NEW_WINDOW,
            "editor.navigate.open-in-new-window", "Open Result in New Window", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_COPY_LOCATION,
            "editor.navigate.copy-location", "Copy Source Location", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_COPY_QUALIFIED_NAME,
            "editor.navigate.copy-qualified-name", "Copy Qualified Symbol Name", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 1, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_REFERENCE_NEXT,
            "editor.navigate.reference-next", "Next Reference", "F4",
            UMI_EDITOR_NAVIGATION_QUERY_REFERENCE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_REFERENCE_PREVIOUS,
            "editor.navigate.reference-previous", "Previous Reference", "Shift+F4",
            UMI_EDITOR_NAVIGATION_QUERY_REFERENCE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_REFERENCE_FOCUS_FILTER,
            "editor.navigate.reference-focus-filter", "Focus Reference Filter",
            "Ctrl+F", UMI_EDITOR_NAVIGATION_QUERY_REFERENCE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_REFERENCE_TOGGLE_DECLARATION,
            "editor.navigate.reference-toggle-declaration",
            "Toggle Declaration in References", "",
            UMI_EDITOR_NAVIGATION_QUERY_REFERENCE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_HIERARCHY_REFRESH,
            "editor.navigate.hierarchy-refresh", "Refresh Hierarchy", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_HIERARCHY_EXPAND_ALL,
            "editor.navigate.hierarchy-expand-all", "Expand Entire Hierarchy", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_HIERARCHY_COLLAPSE_ALL,
            "editor.navigate.hierarchy-collapse-all", "Collapse Entire Hierarchy", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_OUTLINE_FOLLOW_CURSOR,
            "editor.navigate.outline-follow-cursor", "Toggle Outline Follow Cursor", "",
            UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_BREADCRUMB_SELECT_SIBLING,
            "editor.navigate.breadcrumb-select-sibling", "Select Breadcrumb Sibling", "",
            UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL, 0, 0),
    COMMAND(UMI_EDITOR_NAVIGATION_COMMAND_PROVIDER_DIAGNOSTICS,
            "editor.navigate.provider-diagnostics", "Show Navigation Providers", "",
            UMI_EDITOR_NAVIGATION_QUERY_NONE, 0, 1)
};

#undef COMMAND

/*
 * Return the number of records represented by editor navigation command without changing
 * their state.
 */
size_t umi_editor_navigation_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

/*
 * Find editor navigation command while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiEditorNavigationCommandDescriptor *
umi_editor_navigation_command_at(size_t index)
{
    return index < umi_editor_navigation_command_count() ? &COMMANDS[index]
                                                         : NULL;
}

/*
 * Find editor navigation command while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiEditorNavigationCommandDescriptor *
umi_editor_navigation_command_find(const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_editor_navigation_command_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(COMMANDS[index].id, id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

/*
 * Provide the editor navigation command for kind operation used by this module and its
 * client applications.
 */
const UmiEditorNavigationCommandDescriptor *
umi_editor_navigation_command_for_kind(UmiEditorNavigationCommandKind kind)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_editor_navigation_command_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (COMMANDS[index].kind == kind) return &COMMANDS[index];
    }
    return NULL;
}
