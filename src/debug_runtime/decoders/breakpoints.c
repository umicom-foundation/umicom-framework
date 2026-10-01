/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/breakpoints.c
 *
 * PURPOSE:
 *   Decode DAP setBreakpoints-family response into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/breakpoints.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the debug runtime decode breakpoints operation used by this module and its
 * client applications.
 */
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
        ? umi_language_runtime_json_object_get(&document, body, "breakpoints")
        : -1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (array < 0) return UMI_STATUS_PARSE_ERROR;
    if (document.tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY) return UMI_STATUS_PARSE_ERROR;

    count = umi_language_runtime_json_array_count(&document, array);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
/* Truncating a source-set reply could confirm only part of the requested breakpoints. Reject excess evidence before decoding instead. The previous implementation remains for engineering review. */
#if 0
    if (count > UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS) {
        count = UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS;
    }
#endif
    if (count > UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        int source_token;
        int64_t id;
        UmiDebugRuntimeBreakpoint *item =
            &out_result->items[out_result->count];

        /* Preserve the original failure result so the caller can respond to the correct cause. */
/* A missing row or invalid verification/location cannot become a successful source-set confirmation. Validate required booleans and optional signed coordinates before projection. The previous implementation remains for engineering review. */
#if 0
        if (token < 0) continue;
#endif
        if (token < 0 || document.tokens[token].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
        int verifiedToken = umi_language_runtime_json_object_get(&document, token, "verified");
        int verified = 0;
        if (verifiedToken < 0 || umi_language_runtime_json_bool(&document, verifiedToken, &verified) != UMI_STATUS_OK)
            return UMI_STATUS_PARSE_ERROR;
        const char *locations[] = {"line", "column"};
        for (size_t field = 0U; field < 2U; ++field) {
            int location = umi_language_runtime_json_object_get(&document, token, locations[field]);
            int64_t value = 0;
            if (location >= 0 && (umi_language_runtime_json_int64(&document, location, &value) != UMI_STATUS_OK ||
                value <= 0 || value > INT32_MAX)) return UMI_STATUS_PARSE_ERROR;
        }
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
