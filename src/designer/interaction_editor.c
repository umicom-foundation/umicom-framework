/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/interaction_editor.c
 * PURPOSE: Implement semantic interaction mapping edits for Builder v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/interaction_editor.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerInteractionEditor {
    UmiDesignerInteraction items[UMI_DESIGNER_MAX_INTERACTIONS];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDesignerInteractionEditor *editor,
                         const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (editor == NULL || id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < editor->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(editor->items[index].interaction_id, id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise designer interaction editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_interaction_editor_create(
    UmiDesignerInteractionEditor **out_editor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_editor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_editor = calloc(1U, sizeof(**out_editor));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (*out_editor == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (*out_editor)->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by designer interaction editor so the same storage can be
 * reused safely.
 */
void umi_designer_interaction_editor_destroy(
    UmiDesignerInteractionEditor *editor)
{
    free(editor);
}

/*
 * Provide the designer interaction editor upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_interaction_editor_upsert(
    UmiDesignerInteractionEditor *editor,
    const UmiDesignerDocument *document,
    const UmiDesignerInteraction *interaction)
{
    UmiDeclNode node;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (editor == NULL || document == NULL || interaction == NULL ||
        interaction->interaction_id[0] == '\0' ||
        interaction->source_name[0] == '\0' ||
        interaction->command_id[0] == '\0' ||
        interaction->kind < UMI_DESIGNER_INTERACTION_SIGNAL ||
        interaction->kind > UMI_DESIGNER_INTERACTION_COMMAND) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_decl_document_find_node(
        umi_designer_document_declarative((UmiDesignerDocument *)document),
        interaction->node_id, &node);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    index = find_index(editor, interaction->interaction_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (editor->count >= UMI_DESIGNER_MAX_INTERACTIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = editor->count++;
    }
    editor->items[index] = *interaction;
    editor->revision++;
    editor->items[index].revision = editor->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove designer interaction editor while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_interaction_editor_remove(
    UmiDesignerInteractionEditor *editor,
    const char *interaction_id)
{
    size_t index = find_index(editor, interaction_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (editor == NULL || interaction_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < editor->count) {
        (void)memmove(&editor->items[index], &editor->items[index + 1U],
                      (editor->count - index - 1U) * sizeof(editor->items[0]));
    }
    editor->count--;
    editor->revision++;
    return UMI_STATUS_OK;
}

/*
 * Find designer interaction editor while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_interaction_editor_at(
    const UmiDesignerInteractionEditor *editor,
    size_t index,
    UmiDesignerInteraction *out_interaction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (editor == NULL || out_interaction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= editor->count) return UMI_STATUS_NOT_FOUND;
    *out_interaction = editor->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by designer interaction editor without changing
 * their state.
 */
size_t umi_designer_interaction_editor_count(
    const UmiDesignerInteractionEditor *editor)
{
    return editor != NULL ? editor->count : 0U;
}
