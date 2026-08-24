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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/workspace_edit.h"
#include "umicom/language_runtime/decoders/text_edits.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    if (json == NULL || out == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out, 0, sizeof(*out));
    status = umi_language_runtime_json_parse(json, &document);
    if (status != UMI_STATUS_OK) return status;

    result_token = umi_language_runtime_decoder_result_token(&document);
    if (result_token < 0 ||
        umi_language_runtime_json_is_null(&document, result_token)) {
        return UMI_STATUS_OK;
    }

    changes_token = umi_language_runtime_json_object_get(
        &document,
        result_token,
        "changes");
    if (changes_token < 0) return UMI_STATUS_NOT_IMPLEMENTED;

    entry_count = umi_language_runtime_json_object_count(
        &document,
        changes_token);
    if (entry_count == 0U) return UMI_STATUS_OK;

    edits = (UmiLanguageRuntimeTextEditList *)calloc(1U, sizeof(*edits));
    if (edits == NULL) return UMI_STATUS_OUT_OF_MEMORY;

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
        if (status != UMI_STATUS_OK) goto cleanup;

        status = umi_language_runtime_json_string(
            &document,
            key_token,
            uri,
            sizeof(uri));
        if (status != UMI_STATUS_OK) goto cleanup;

        /* The same heap block is safely reused for each URI in the map. */
        (void)memset(edits, 0, sizeof(*edits));
        status = umi_language_runtime_decode_text_edit_array_token(
            &document,
            value_token,
            edits);
        if (status != UMI_STATUS_OK) goto cleanup;

        for (edit_index = 0U; edit_index < edits->count; ++edit_index) {
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
