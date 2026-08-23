/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/exception_info.c
 *
 * PURPOSE:
 *   Decode DAP exceptionInfo response into bounded Framework runtime records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/exception_info.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_debug_runtime_decode_exception_info(
    const char *json,
    UmiDebugRuntimeExceptionInfo *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    int details_token;
    UmiStatus status;

    if (json == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    status = umi_language_runtime_json_parse(json, &document);
    if (status != UMI_STATUS_OK) return status;
    body = umi_debug_runtime_decoder_body_token(&document);
    if (body < 0) return UMI_STATUS_PARSE_ERROR;

    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "exceptionId",
        out_result->exception_id, sizeof(out_result->exception_id));
    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "description",
        out_result->description, sizeof(out_result->description));
    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "breakMode",
        out_result->break_mode, sizeof(out_result->break_mode));

    details_token = umi_language_runtime_json_object_get(
        &document, body, "details");
    if (details_token >= 0) {
        (void)umi_debug_runtime_decoder_optional_string(
            &document, details_token, "message",
            out_result->details, sizeof(out_result->details));
    }

    return UMI_STATUS_OK;
}
