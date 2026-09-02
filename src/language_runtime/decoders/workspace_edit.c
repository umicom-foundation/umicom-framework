/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/workspace_edit.c
 *
 * PURPOSE:
 *   Decode common WorkspaceEdit.changes maps for rename/refactor preview.
 *
 * MEMORY POLICY:
 *   UmiLanguageRuntimeTextEditList is deliberately large because it owns every
 *   decoded replacement string.  It must not be placed on the comparatively
 *   small default Windows executable stack.  Phase 5 therefore uses controlled
 *   heap storage for the temporary list and releases it on every exit path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/workspace_edit.h"
#include "umicom/language_runtime/decoders/text_edits.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Provide the language runtime decode workspace edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_workspace_edit(
    const char *json,
    UmiLanguageRuntimeWorkspaceEdit *out)
{
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeTextEditList *edits = NULL;
    int result_token;
    int changes_token;
    size_t entry_index;
    size_t entry_count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL || out == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out, 0, sizeof(*out));
    status = umi_language_runtime_json_parse(json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    result_token = umi_language_runtime_decoder_result_token(&document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result_token < 0 ||
        umi_language_runtime_json_is_null(&document, result_token)) {
        return UMI_STATUS_OK;
    }

    changes_token = umi_language_runtime_json_object_get(
        &document,
        result_token,
        "changes");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (changes_token < 0) return UMI_STATUS_NOT_IMPLEMENTED;

    entry_count = umi_language_runtime_json_object_count(
        &document,
        changes_token);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (entry_count == 0U) return UMI_STATUS_OK;

    edits = (UmiLanguageRuntimeTextEditList *)calloc(1U, sizeof(*edits));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edits == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    /* Visit each bounded item once so every record receives the same rule. */
    for (entry_index = 0U; entry_index < entry_count; ++entry_index) {
        int key_token = -1;
        int value_token = -1;
        char uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];
        size_t edit_index;

        status = umi_language_runtime_json_object_entry_at(
            &document,
            changes_token,
            entry_index,
            &key_token,
            &value_token);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto cleanup;

        status = umi_language_runtime_json_string(
            &document,
            key_token,
            uri,
            sizeof(uri));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto cleanup;

        /* The same heap block is safely reused for each URI in the map. */
        (void)memset(edits, 0, sizeof(*edits));
        status = umi_language_runtime_decode_text_edit_array_token(
            &document,
            value_token,
            edits);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto cleanup;

        /* Visit each bounded item once so every record receives the same rule. */
        for (edit_index = 0U; edit_index < edits->count; ++edit_index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (out->count >= UMI_LANGUAGE_RUNTIME_MAX_EDITS) {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                goto cleanup;
            }

            (void)snprintf(
                out->items[out->count].uri,
                sizeof(out->items[out->count].uri),
                "%s",
                uri);
            out->items[out->count].edit = edits->edits[edit_index];
            out->count += 1U;
        }
    }

    status = UMI_STATUS_OK;

cleanup:
    free(edits);
    return status;
}
