/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/scopes.c
 *
 * PURPOSE:
 *   Decode DAP scopes response into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/scopes.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the debug runtime decode scopes operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decode_scopes(
    const char *json,
    UmiDebugRuntimeScopeList *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    int array;
    size_t index;
    size_t count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    status = umi_language_runtime_json_parse(json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    body = umi_debug_runtime_decoder_body_token(&document);
    array = body >= 0
        ? umi_language_runtime_json_object_get(&document, body, "scopes")
        : -1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    count = umi_language_runtime_json_array_count(&document, array);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_DEBUG_RUNTIME_MAX_ITEMS) count = UMI_DEBUG_RUNTIME_MAX_ITEMS;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        int64_t reference;
        UmiDebugRuntimeScope *item = &out_result->items[out_result->count];

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (token < 0) continue;
        reference = umi_debug_runtime_decoder_optional_int(
            &document, token, "variablesReference", 0);
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "name", item->name, sizeof(item->name));
        item->variables_reference =
            reference > 0 ? (uint64_t)reference : 0U;
        item->named_variables = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "namedVariables", 0);
        item->indexed_variables = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "indexedVariables", 0);
        item->expensive = umi_debug_runtime_decoder_optional_bool(
            &document, token, "expensive", 0);
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "presentationHint",
            item->presentation_hint, sizeof(item->presentation_hint));
        out_result->count += 1U;
    }

    return UMI_STATUS_OK;
}
