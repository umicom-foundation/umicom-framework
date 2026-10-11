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
#include <stdlib.h>
#include "../response_fields_internal.h"
#include <string.h>

/*
 * Provide the debug runtime decode stack trace operation used by this module and its
 * client applications.
 */
/* Stack decoding now stages and validates the complete response so malformed rows cannot silently change stack positions or truncate frame identities. The earlier decoder and its comments remain for engineering review. The earlier implementation is retained below for engineering review. */
#if 0
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
        ? umi_language_runtime_json_object_get(&document, body, "stackFrames")
        : -1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    total = umi_debug_runtime_decoder_optional_int(
        &document, body, "totalFrames", 0);
    out_result->total_frames = total > 0 ? (size_t)total : 0U;

    count = umi_language_runtime_json_array_count(&document, array);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_DEBUG_RUNTIME_MAX_ITEMS) count = UMI_DEBUG_RUNTIME_MAX_ITEMS;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        int source_token;
        int64_t id;
        UmiDebugRuntimeStackFrame *item =
            &out_result->items[out_result->count];

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (token < 0) continue;
        /* DAP frame identifiers may be zero. A missing/negative identifier is
         * different from the valid top frame 0. The previous test below lost
         * that frame before the shared platform could request its scopes.
         * Replacement: this decoder keeps the complete non-negative DAP range.
         */
        // id = umi_debug_runtime_decoder_optional_int(&document, token, "id", 0);
        // if (id <= 0) continue;
        id = umi_debug_runtime_decoder_optional_int(&document, token, "id", -1);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (id < 0 || id > INT32_MAX) continue;

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

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (out_result->total_frames == 0U) out_result->total_frames = out_result->count;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_debug_runtime_decode_stack_trace(const char *json,
                                               UmiDebugRuntimeStackTrace *out_result)
{
    if (json == NULL || out_result == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve stack positions: dropping a malformed row would renumber the
     * remaining frames and make a later startFrame request refer to another call. */
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    UmiDebugRuntimeStackTrace *result = calloc(1U, sizeof *result);
    if (document == NULL || result == NULL)
    {
        free(document);
        free(result);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1, array = -1, known = 0;
    uint64_t total = 0U;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonResponse(document, "stackTrace", 0, &body);
    if (status == UMI_STATUS_OK)
    {
        array = DebugResponseJsonField(document, body, "stackFrames");
        if (array < 0 || document->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK)
    {
        result->count = umi_language_runtime_json_array_count(document, array);
        if (result->count > UMI_DEBUG_RUNTIME_MAX_ITEMS)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
/* The protocol defines totalFrames as an unsigned 32-bit count. This replaces the signed bound while retaining the earlier validation for review. The earlier implementation is retained below for engineering review. */
#if 0
    if (status == UMI_STATUS_OK)
        status =
            DebugResponseJsonUnsigned(document, body, "totalFrames", 0, INT32_MAX, &total, &known);
#endif
    /* DAP totalFrames is unsigned metadata, independent of signed frame IDs. */
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonUnsigned(document, body, "totalFrames", 0,
                                           UINT32_MAX, &total, &known);
    if (status == UMI_STATUS_OK && total != 0U && total < result->count)
        status = UMI_STATUS_PARSE_ERROR;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < result->count; ++index)
    {
        int row = umi_language_runtime_json_array_at(document, array, index);
        UmiDebugRuntimeStackFrame *frame = &result->items[index];
        status = DebugResponseJsonObject(document, row);
        if (status == UMI_STATUS_OK)
            status =
                DebugResponseJsonUnsigned(document, row, "id", 1, INT32_MAX, &frame->id, &known);
        if (status == UMI_STATUS_OK)
            status =
                DebugResponseJsonString(document, row, "name", 1, frame->name, sizeof frame->name);
        uint64_t line = 0U, column = 0U;
        if (status == UMI_STATUS_OK)
            status = DebugResponseJsonUnsigned(document, row, "line", 0, INT64_MAX, &line, &known);
        if (status == UMI_STATUS_OK)
            status =
                DebugResponseJsonUnsigned(document, row, "column", 0, INT64_MAX, &column, &known);
        if (status == UMI_STATUS_OK && (line > UINT32_MAX || column > UINT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK)
        {
            frame->line = (uint32_t)line;
            frame->column = (uint32_t)column;
            status = DebugResponseJsonString(document, row, "instructionPointerReference", 0,
                                             frame->instruction_pointer,
                                             sizeof frame->instruction_pointer);
        }
        if (status == UMI_STATUS_OK)
            status =
                DebugResponseJsonString(document, row, "presentationHint", 0,
                                        frame->presentation_hint, sizeof frame->presentation_hint);
        int source = DebugResponseJsonField(document, row, "source");
        if (status == UMI_STATUS_OK && source >= 0)
            status = DebugResponseJsonSource(document, source, &frame->source);
        /* Frame IDs are connection-scoped identities, not array positions.
         * Duplicate identities cannot safely populate the canonical registry. */
        for (size_t previous = 0U; status == UMI_STATUS_OK && previous < index; ++previous)
            if (result->items[previous].id == frame->id)
                status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK)
    {
        result->total_frames = total != 0U ? (size_t)total : result->count;
        *out_result = *result;
    }
    free(document);
    free(result);
    return status;
}
