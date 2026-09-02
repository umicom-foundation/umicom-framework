/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/workspace_edit_bridge.c
 *
 * PURPOSE:
 *   Project LSP rename/refactor WorkspaceEdit records into the established
 *   Framework Editor validation/apply pipeline without inventing byte offsets
 *   or expected source text that the protocol did not supply.
 *
 * ARCHITECTURE:
 *   LSP positions are zero-based UTF-16 line/character coordinates. The bridge
 *   stages them as UNRESOLVED Editor edits. The Editor module later resolves
 *   those coordinates against the authoritative UTF-8 text buffer, captures
 *   the expected text/revision and only then permits application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language_runtime/workspace_edit_bridge.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the language runtime workspace edit to editor operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_workspace_edit_to_editor(
    const UmiLanguageRuntimeWorkspaceEdit *runtime_edit,
    UmiEditorWorkspaceEditSet **out_edit_set)
{
    UmiEditorWorkspaceEditSet *edit_set = NULL;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime_edit == NULL || out_edit_set == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_edit_set = NULL;
    status = umi_editor_workspace_edit_set_create(&edit_set);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime_edit->count; ++index) {
        const UmiLanguageRuntimeWorkspaceEditItem *source =
            &runtime_edit->items[index];
        UmiEditorWorkspaceTextEdit target;
        size_t replacement_length;

        (void)memset(&target, 0, sizeof(target));
        replacement_length = strlen(source->edit.new_text);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (replacement_length >= sizeof(target.replacement_text)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto failure;
        }

        target.struct_size = (uint32_t)sizeof(target);
        target.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
        (void)snprintf(target.id, sizeof(target.id), "lsp.edit.%zu", index);
        (void)snprintf(target.provider_id, sizeof(target.provider_id), "lsp");

        status = umi_editor_source_location_initialize(
            &target.location,
            source->uri,
            source->edit.range.start.line,
            source->edit.range.start.character);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        target.location.kind = UMI_EDITOR_SOURCE_LOCATION_EDIT;
        target.location.end_line = source->edit.range.end.line;
        target.location.end_column = source->edit.range.end.character;
        (void)memcpy(
            target.replacement_text,
            source->edit.new_text,
            replacement_length + 1U);

        /*
         * The protocol does not carry authoritative byte offsets, source
         * revision or expected text. Mark that fact explicitly instead of
         * fabricating safety data merely to satisfy the native edit validator.
         */
        target.state = UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED;
        target.required = 1;

        status = umi_editor_workspace_edit_set_upsert_unresolved(
            edit_set,
            &target);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;
    }

    status = umi_editor_workspace_edit_set_finalize(edit_set);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    *out_edit_set = edit_set;
    return UMI_STATUS_OK;

failure:
    umi_editor_workspace_edit_set_destroy(edit_set);
    return status;
}
