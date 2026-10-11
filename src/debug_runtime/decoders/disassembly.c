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
#include <stdlib.h>
#include "../instruction_fields_internal.h"
#include <string.h>

/*
 * Provide the debug runtime decode disassembly operation used by this module and its
 * client applications.
 */
/* Disassembly now shares strict field validation and heap-backed staging with captured instruction pages. Oversized or malformed replies fail without publishing a truncated or partial result. The earlier implementation is retained below for engineering review. */
#if 0
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
#endif
UmiStatus umi_debug_runtime_decode_disassembly(const char *json,
                                               UmiDebugRuntimeDisassembly *out_result)
{
    if (json == NULL || out_result == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugInstructionRecord *rows = calloc(UMI_DEBUG_RUNTIME_MAX_INSTRUCTIONS, sizeof *rows);
    UmiDebugRuntimeDisassembly *result = calloc(1U, sizeof *result);
    if (rows == NULL || result == NULL)
    {
        free(rows);
        free(result);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status =
        DebugInstructionArrayDecode(json, rows, UMI_DEBUG_RUNTIME_MAX_INSTRUCTIONS, &result->count);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < result->count; ++i)
    {
        UmiDebugRuntimeInstruction *item = &result->items[i];
        if (rows[i].line > UINT32_MAX || rows[i].column > UINT32_MAX)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        /* The existing result fields retain their order. Rich location metadata
         * stays available through InstructionPageDecode for newer viewers. */
        memcpy(item->address, rows[i].address, sizeof item->address);
        memcpy(item->instruction_bytes, rows[i].bytes, sizeof item->instruction_bytes);
        memcpy(item->instruction, rows[i].text, sizeof item->instruction);
        memcpy(item->symbol, rows[i].symbol, sizeof item->symbol);
        memcpy(item->presentation_hint, rows[i].presentation_hint, sizeof item->presentation_hint);
        item->location = rows[i].source;
        item->line = (uint32_t)rows[i].line;
        item->column = (uint32_t)rows[i].column;
    }
    if (status == UMI_STATUS_OK)
        *out_result = *result;
    free(result);
    free(rows);
    return status;
}
