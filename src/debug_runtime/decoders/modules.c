/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/modules.c
 *
 * PURPOSE:
 *   Decode DAP modules response into bounded Framework runtime records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/modules.h"

#include <stdio.h>
#include <string.h>

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

    if (json == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    status = umi_language_runtime_json_parse(json, &document);
    if (status != UMI_STATUS_OK) return status;
    body = umi_debug_runtime_decoder_body_token(&document);
    array = body >= 0
        ? umi_language_runtime_json_object_get(&document, body, "modules")
        : -1;
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    total = umi_debug_runtime_decoder_optional_int(
        &document, body, "totalModules", 0);
    out_result->total_modules = total > 0 ? (size_t)total : 0U;

    count = umi_language_runtime_json_array_count(&document, array);
    if (count > UMI_DEBUG_RUNTIME_MAX_ITEMS) count = UMI_DEBUG_RUNTIME_MAX_ITEMS;

    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        UmiDebugRuntimeModule *item = &out_result->items[out_result->count];

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

    if (out_result->total_modules == 0U) out_result->total_modules = out_result->count;
    return UMI_STATUS_OK;
}
