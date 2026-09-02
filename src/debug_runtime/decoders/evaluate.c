/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/evaluate.c
 *
 * PURPOSE:
 *   Decode DAP evaluate/setVariable/setExpression response into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/evaluate.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the debug runtime decode evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decode_evaluate(
    const char *json,
    UmiDebugRuntimeEvaluateResult *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    int64_t reference;
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
        &document, body, "result", out_result->result, sizeof(out_result->result));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_result->result[0] == '\0') {
        (void)umi_debug_runtime_decoder_optional_string(
            &document, body, "value", out_result->result, sizeof(out_result->result));
    }
    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "type", out_result->type, sizeof(out_result->type));
    (void)umi_debug_runtime_decoder_optional_string(
        &document, body, "memoryReference",
        out_result->memory_reference, sizeof(out_result->memory_reference));
    reference = umi_debug_runtime_decoder_optional_int(
        &document, body, "variablesReference", 0);
    out_result->variables_reference =
        reference > 0 ? (uint64_t)reference : 0U;
    out_result->named_variables =
        (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, body, "namedVariables", 0);
    out_result->indexed_variables =
        (uint32_t)umi_debug_runtime_decoder_optional_int(
            &document, body, "indexedVariables", 0);
    return UMI_STATUS_OK;
}
