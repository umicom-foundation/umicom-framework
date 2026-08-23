/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/memory.c
 *
 * PURPOSE:
 *   Decode DAP readMemory response into bounded Framework runtime records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/memory.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_debug_runtime_decode_memory(
    const char *json,
    UmiDebugRuntimeMemoryResult *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    char data[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    int64_t unreadable;
    UmiStatus status;

    if (json == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    status = umi_language_runtime_json_parse(json, &document);
    if (status != UMI_STATUS_OK) return status;
    body = umi_debug_runtime_decoder_body_token(&document);
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
    if (status != UMI_STATUS_OK) return status;

    if (data[0] == '\0') return UMI_STATUS_OK;

    return umi_debug_runtime_base64_decode(
        data,
        out_result->bytes,
        sizeof(out_result->bytes),
        &out_result->byte_count);
}
