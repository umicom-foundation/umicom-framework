/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_command.c
 *
 * PURPOSE:
 *   Implement the canonical Framework-owned editor-intelligence command
 *   catalogue consumed by Studio and future Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_command.h"

#include <string.h>

#define COMMAND(command_kind, command_id, command_label, key, symbol, editable, view) \
    {                                                                                  \
        (uint32_t)sizeof(UmiEditorIntelligenceCommandDescriptor),                      \
        UMI_EDITOR_INTELLIGENCE_COMMAND_API_VERSION, command_kind, command_id,         \
        command_label, key, symbol, editable, view                                     \
    }

static const UmiEditorIntelligenceCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_RENAME_SYMBOL,
            "editor.intelligence.rename", "Rename Symbol", "F2", 1, 1, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_PREVIEW_RENAME,
            "editor.intelligence.rename-preview", "Preview Rename", "", 1, 1, 1),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_APPLY_RENAME,
            "editor.intelligence.rename-apply", "Apply Rename", "", 0, 1, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_CANCEL_RENAME,
            "editor.intelligence.rename-cancel", "Cancel Rename", "Escape", 0, 0, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_TOGGLE_CODE_LENS,
            "editor.intelligence.code-lens.toggle", "Toggle CodeLens", "", 0, 0, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_REFRESH_CODE_LENS,
            "editor.intelligence.code-lens.refresh", "Refresh CodeLens", "", 0, 0, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_EXECUTE_CODE_LENS,
            "editor.intelligence.code-lens.execute", "Execute CodeLens", "", 0, 0, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_TOGGLE_INLAY_HINTS,
            "editor.intelligence.inlay-hints.toggle", "Toggle Inlay Hints", "", 0, 0, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_REFRESH_INLAY_HINTS,
            "editor.intelligence.inlay-hints.refresh", "Refresh Inlay Hints", "", 0, 0, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_TOGGLE_SEMANTIC_HIGHLIGHTING,
            "editor.intelligence.semantic.toggle", "Toggle Semantic Highlighting", "", 0, 0, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_REFRESH_SEMANTIC_HIGHLIGHTING,
            "editor.intelligence.semantic.refresh", "Refresh Semantic Highlighting", "", 0, 0, 0),
    COMMAND(UMI_EDITOR_INTELLIGENCE_COMMAND_INSPECT_SEMANTIC_TOKEN,
            "editor.intelligence.semantic.inspect", "Inspect Semantic Token", "", 0, 0, 1)
};

#undef COMMAND

/*
 * Return the number of records represented by editor intelligence command without changing
 * their state.
 */
size_t umi_editor_intelligence_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

/*
 * Find editor intelligence command while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiEditorIntelligenceCommandDescriptor *
umi_editor_intelligence_command_at(size_t index)
{
    return index < umi_editor_intelligence_command_count() ? &COMMANDS[index]
                                                           : NULL;
}

/*
 * Find editor intelligence command while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiEditorIntelligenceCommandDescriptor *
umi_editor_intelligence_command_find(const char *id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_editor_intelligence_command_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(COMMANDS[index].id, id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

/*
 * Provide the editor intelligence command for kind operation used by this module and its
 * client applications.
 */
const UmiEditorIntelligenceCommandDescriptor *
umi_editor_intelligence_command_for_kind(
    UmiEditorIntelligenceCommandKind kind)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_editor_intelligence_command_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (COMMANDS[index].kind == kind) return &COMMANDS[index];
    }
    return NULL;
}
