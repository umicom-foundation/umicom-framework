/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/modules.c
 *
 * PURPOSE:
 *   Decode DAP modules response into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/modules.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the debug runtime decode modules operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decode_modules(
    const char *json,
    UmiDebugRuntimeModuleList *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    int array;
    size_t index;
    size_t count;
    int64_t total;
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
        ? umi_language_runtime_json_object_get(&document, body, "modules")
        : -1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    total = umi_debug_runtime_decoder_optional_int(
        &document, body, "totalModules", 0);
    out_result->total_modules = total > 0 ? (size_t)total : 0U;

    count = umi_language_runtime_json_array_count(&document, array);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_DEBUG_RUNTIME_MAX_ITEMS) count = UMI_DEBUG_RUNTIME_MAX_ITEMS;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        UmiDebugRuntimeModule *item = &out_result->items[out_result->count];

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (token < 0) continue;
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "id", item->id, sizeof(item->id));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "name", item->name, sizeof(item->name));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "path", item->path, sizeof(item->path));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "version", item->version, sizeof(item->version));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "symbolStatus",
            item->symbol_status, sizeof(item->symbol_status));
        item->optimized = umi_debug_runtime_decoder_optional_bool(
            &document, token, "isOptimized", 0);
        out_result->count += 1U;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (out_result->total_modules == 0U) out_result->total_modules = out_result->count;
    return UMI_STATUS_OK;
}
