/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/memory.c
 *
 * PURPOSE:
 *   Decode DAP readMemory response into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/memory.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the debug runtime decode memory operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decode_memory(
    const char *json,
    UmiDebugRuntimeMemoryResult *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    char data[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    int64_t unreadable;
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (body < 0) return UMI_STATUS_PARSE_ERROR;

    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "address",
        out_result->address, sizeof(out_result->address));
    unreadable = umi_debug_runtime_decoder_optional_int(
        &document, body, "unreadableBytes", 0);
    out_result->unreadable_bytes =
        unreadable > 0 ? (size_t)unreadable : 0U;

    status = umi_debug_runtime_decoder_optional_string(
        &document, body, "data", data, sizeof(data));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (data[0] == '\0') return UMI_STATUS_OK;

    return umi_debug_runtime_base64_decode(
        data,
        out_result->bytes,
        sizeof(out_result->bytes),
        &out_result->byte_count);
}
