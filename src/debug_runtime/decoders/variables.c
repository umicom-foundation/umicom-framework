/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/variables.c
 *
 * PURPOSE:
 *   Decode DAP variables response into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/variables.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_debug_runtime_decode_variables(
    const char *json,
    UmiDebugRuntimeVariableList *out_result)
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
        ? umi_language_runtime_json_object_get(&document, body, "variables")
        : -1;
    if (array < 0) return UMI_STATUS_PARSE_ERROR;

    count = umi_language_runtime_json_array_count(&document, array);
    if (count > UMI_DEBUG_RUNTIME_MAX_VARIABLES) {
        count = UMI_DEBUG_RUNTIME_MAX_VARIABLES;
    }

    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(&document, array, index);
        int64_t reference;
        UmiDebugRuntimeVariable *item =
            &out_result->items[out_result->count];

        if (token < 0) continue;
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "name", item->name, sizeof(item->name));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "value", item->value, sizeof(item->value));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "type", item->type, sizeof(item->type));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "evaluateName",
            item->evaluate_name, sizeof(item->evaluate_name));
        (void)umi_debug_runtime_decoder_optional_string(
            &document, token, "memoryReference",
            item->memory_reference, sizeof(item->memory_reference));
        reference = umi_debug_runtime_decoder_optional_int(
            &document, token, "variablesReference", 0);
        item->variables_reference =
            reference > 0 ? (uint64_t)reference : 0U;
        item->named_variables = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "namedVariables", 0);
        item->indexed_variables = (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, token, "indexedVariables", 0);
        out_result->count += 1U;
    }

    return UMI_STATUS_OK;
}
