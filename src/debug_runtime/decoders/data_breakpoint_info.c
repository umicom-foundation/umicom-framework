/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/data_breakpoint_info.c
 *
 * PURPOSE:
 *   Decode DAP dataBreakpointInfo response into bounded Framework runtime records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/data_breakpoint_info.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_debug_runtime_decode_data_breakpoint_info(
    const char *json,
    UmiDebugRuntimeDataBreakpointInfo *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    int access_token;
    size_t index;
    size_t count;
    UmiLanguageRuntimeJsonWriter writer;
    UmiStatus status;

    if (json == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    status = umi_language_runtime_json_parse(json, &document);
    if (status != UMI_STATUS_OK) return status;
    body = umi_debug_runtime_decoder_body_token(&document);
    if (body < 0) return UMI_STATUS_PARSE_ERROR;

    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "dataId",
        out_result->data_id, sizeof(out_result->data_id));
    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "description",
        out_result->description, sizeof(out_result->description));
    out_result->can_persist = umi_debug_runtime_decoder_optional_bool(
        &document, body, "canPersist", 0);

    access_token = umi_language_runtime_json_object_get(
        &document, body, "accessTypes");
    if (access_token >= 0) {
        umi_language_runtime_json_writer_init(
            &writer,
            out_result->access_types,
            sizeof(out_result->access_types));
        count = umi_language_runtime_json_array_count(&document, access_token);

        for (index = 0U; index < count; ++index) {
            int token = umi_language_runtime_json_array_at(
                &document, access_token, index);
            char value[64];

            if (token < 0 ||
                umi_language_runtime_json_string(
                    &document, token, value, sizeof(value)) != UMI_STATUS_OK) {
                continue;
            }

            if (index > 0U) {
                (void)umi_language_runtime_json_writer_raw(&writer, ",");
            }
            (void)umi_language_runtime_json_writer_raw(&writer, value);
        }

        if (writer.status != UMI_STATUS_OK) return writer.status;
    }

    return UMI_STATUS_OK;
}
