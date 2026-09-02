/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/disassembly.c
 *
 * PURPOSE:
 *   Decode DAP disassemble response into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/disassembly.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the debug runtime decode disassembly operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_decode_disassembly(
    const char *json,
    UmiDebugRuntimeDisassembly *out_result)
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
        ? umi_language_runtime_json_object_get(&document, body, "instructions")
        : -1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    count = umi_language_runtime_json_array_count(&document, array);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_DEBUG_RUNTIME_MAX_INSTRUCTIONS) {
        count = UMI_DEBUG_RUNTIME_MAX_INSTRUCTIONS;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        int location_token;
        UmiDebugRuntimeInstruction *item =
            &out_result->items[out_result->count];

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (token < 0) continue;
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "address",
            item->address, sizeof(item->address));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "instructionBytes",
            item->instruction_bytes, sizeof(item->instruction_bytes));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "instruction",
            item->instruction, sizeof(item->instruction));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "symbol",
            item->symbol, sizeof(item->symbol));
        item->line = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "line", 0);
        item->column = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "column", 0);
        location_token = umi_language_runtime_json_object_get(
            &document, token, "location");
        (void)umi_debug_runtime_decoder_source(
            &document, location_token, &item->location);
        out_result->count += 1U;
    }

    return UMI_STATUS_OK;
}
