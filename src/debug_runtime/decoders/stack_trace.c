/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/stack_trace.c
 *
 * PURPOSE:
 *   Decode DAP stackTrace response into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/stack_trace.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_debug_runtime_decode_stack_trace(
    const char *json,
    UmiDebugRuntimeStackTrace *out_result)
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
        ? umi_language_runtime_json_object_get(&document, body, "stackFrames")
        : -1;
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    total = umi_debug_runtime_decoder_optional_int(
        &document, body, "totalFrames", 0);
    out_result->total_frames = total > 0 ? (size_t)total : 0U;

    count = umi_language_runtime_json_array_count(&document, array);
    if (count > UMI_DEBUG_RUNTIME_MAX_ITEMS) count = UMI_DEBUG_RUNTIME_MAX_ITEMS;

    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        int source_token;
        int64_t id;
        UmiDebugRuntimeStackFrame *item =
            &out_result->items[out_result->count];

        if (token < 0) continue;
        id = umi_debug_runtime_decoder_optional_int(&document, token, "id", 0);
        if (id <= 0) continue;

        item->id = (uint64_t)id;
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "name", item->name, sizeof(item->name));
        item->line = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "line", 0);
        item->column = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "column", 0);
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "instructionPointerReference",
            item->instruction_pointer, sizeof(item->instruction_pointer));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "presentationHint",
            item->presentation_hint, sizeof(item->presentation_hint));
        source_token = umi_language_runtime_json_object_get(
            &document, token, "source");
        (void)umi_debug_runtime_decoder_source(
            &document, source_token, &item->source);

        out_result->count += 1U;
    }

    if (out_result->total_frames == 0U) out_result->total_frames = out_result->count;
    return UMI_STATUS_OK;
}
