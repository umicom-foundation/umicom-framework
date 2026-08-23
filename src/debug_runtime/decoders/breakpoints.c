/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/breakpoints.c
 *
 * PURPOSE:
 *   Decode DAP setBreakpoints-family response into bounded Framework runtime records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/breakpoints.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_debug_runtime_decode_breakpoints(
    const char *json,
    UmiDebugRuntimeBreakpointList *out_result)
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
        ? umi_language_runtime_json_object_get(&document, body, "breakpoints")
        : -1;
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    count = umi_language_runtime_json_array_count(&document, array);
    if (count > UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS) {
        count = UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS;
    }

    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        int source_token;
        int64_t id;
        UmiDebugRuntimeBreakpoint *item =
            &out_result->items[out_result->count];

        if (token < 0) continue;
        id = umi_debug_runtime_decoder_optional_int(&document, token, "id", 0);
        item->id = id > 0 ? (uint64_t)id : 0U;
        item->verified = umi_debug_runtime_decoder_optional_bool(
            &document, token, "verified", 0);
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "message", item->message, sizeof(item->message));
        item->line = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "line", 0);
        item->column = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "column", 0);
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "instructionReference",
            item->instruction_reference, sizeof(item->instruction_reference));
        item->offset = umi_debug_runtime_decoder_optional_int(
            &document, token, "offset", 0);
        source_token = umi_language_runtime_json_object_get(
            &document, token, "source");
        (void)umi_debug_runtime_decoder_source(
            &document, source_token, &item->source);
        out_result->count += 1U;
    }
    return UMI_STATUS_OK;
}
