/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/threads.c
 *
 * PURPOSE:
 *   Decode DAP threads response into bounded Framework runtime records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/threads.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_debug_runtime_decode_threads(
    const char *json,
    UmiDebugRuntimeThreadList *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    int array;
    size_t index;
    size_t count;
    UmiStatus status;

    if (json == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    status = umi_language_runtime_json_parse(json, &document);
    if (status != UMI_STATUS_OK) return status;
    body = umi_debug_runtime_decoder_body_token(&document);
    array = body >= 0
        ? umi_language_runtime_json_object_get(&document, body, "threads")
        : -1;
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    count = umi_language_runtime_json_array_count(&document, array);
    if (count > UMI_DEBUG_RUNTIME_MAX_ITEMS) count = UMI_DEBUG_RUNTIME_MAX_ITEMS;

    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        int64_t id;
        UmiDebugRuntimeThread *item = &out_result->items[out_result->count];
        if (token < 0) continue;
        id = umi_debug_runtime_decoder_optional_int(&document, token, "id", 0);
        if (id <= 0) continue;
        item->id = (uint64_t)id;
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "name", item->name, sizeof(item->name));
        out_result->count += 1U;
    }
    return UMI_STATUS_OK;
}
