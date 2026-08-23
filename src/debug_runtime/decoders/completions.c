/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/completions.c
 *
 * PURPOSE:
 *   Decode DAP completions response into bounded Framework runtime records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/completions.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_debug_runtime_decode_completions(
    const char *json,
    UmiDebugRuntimeCompletionList *out_result)
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
        ? umi_language_runtime_json_object_get(&document, body, "targets")
        : -1;
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    count = umi_language_runtime_json_array_count(&document, array);
    if (count > UMI_DEBUG_RUNTIME_MAX_ITEMS) count = UMI_DEBUG_RUNTIME_MAX_ITEMS;

    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        UmiDebugRuntimeCompletionTarget *item =
            &out_result->items[out_result->count];

        if (token < 0) continue;
        if (umi_debug_runtime_decoder_optional_string(
                &document, token, "label",
                item->label, sizeof(item->label)) != UMI_STATUS_OK ||
            item->label[0] == '\0') {
            continue;
        }

        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "text", item->text, sizeof(item->text));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "sortText",
            item->sort_text, sizeof(item->sort_text));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "type", item->type, sizeof(item->type));
        item->start = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "start", 0);
        item->length = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "length", 0);
        item->selection_start =
            (uint32_t)umi_debug_runtime_decoder_optional_int(
                &document, token, "selectionStart", 0);
        item->selection_length =
            (uint32_t)umi_debug_runtime_decoder_optional_int(
                &document, token, "selectionLength", 0);

        if (item->text[0] == '\0') {
            const size_t label_length = strlen(item->label);
            const size_t copy_length =
                label_length < sizeof(item->text) - 1U
                    ? label_length
                    : sizeof(item->text) - 1U;

            (void)memcpy(item->text, item->label, copy_length);
            item->text[copy_length] = '\0';
        }

        out_result->count += 1U;
    }

    return UMI_STATUS_OK;
}
