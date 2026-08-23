/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/source.c
 *
 * PURPOSE:
 *   Decode DAP source response into bounded Framework runtime records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/source.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_debug_runtime_decode_source(
    const char *json,
    UmiDebugRuntimeSourceResult *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    UmiStatus status;

    if (json == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    status = umi_language_runtime_json_parse(json, &document);
    if (status != UMI_STATUS_OK) return status;
    body = umi_debug_runtime_decoder_body_token(&document);
    if (body < 0) return UMI_STATUS_PARSE_ERROR;

    status = umi_debug_runtime_decoder_optional_string(
        &document, body, "content",
        out_result->content, sizeof(out_result->content));
    if (status != UMI_STATUS_OK) return status;

    return umi_debug_runtime_decoder_optional_string(
        &document, body, "mimeType",
        out_result->mime_type, sizeof(out_result->mime_type));
}
