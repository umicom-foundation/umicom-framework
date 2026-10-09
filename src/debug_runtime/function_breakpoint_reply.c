/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/function_breakpoint_reply.c
 * PURPOSE: Read exact function-breakpoint verification without trusting truncated reply fields.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "response_fields_internal.h"
#include "umicom/debug_runtime/function_breakpoint_session.h"
#include <limits.h>
#include <stdlib.h>
static UmiStatus FunctionNumber(const UmiLanguageRuntimeJsonDocument *document, int object,
                                const char *key, int64_t maximum, int64_t *out)
{
    int token = DebugResponseJsonField(document, object, key);
    if (token < 0)
    {
        *out = 0;
        return UMI_STATUS_OK;
    }
    UmiStatus status = umi_language_runtime_json_int64(document, token, out);
    if (status != UMI_STATUS_OK || *out < 0 || *out > maximum)
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugFunctionReplyDecode(const char *json, size_t expected_count,
                                      UmiDebugFunctionReply *out)
{
    if (json == NULL || out == NULL || expected_count > UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    UmiDebugFunctionReply *reply = calloc(1U, sizeof *reply);
    if (document == NULL || reply == NULL)
    {
        free(document);
        free(reply);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1, array = -1;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonResponse(document, "setFunctionBreakpoints", 0, &body);
    if (status == UMI_STATUS_OK)
    {
        array = DebugResponseJsonField(document, body, "breakpoints");
        if (array < 0 || document->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
            umi_language_runtime_json_array_count(document, array) != expected_count)
            status = UMI_STATUS_PARSE_ERROR;
    }
    reply->count = expected_count;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < expected_count; ++i)
    {
        int item = umi_language_runtime_json_array_at(document, array, i);
        UmiDebugFunctionVerification *result = &reply->entries[i];
        status = DebugResponseJsonObject(document, item);
        if (status == UMI_STATUS_OK)
            status = DebugResponseJsonBoolean(document, item, "verified", 1, &result->verified);
        if (status == UMI_STATUS_OK)
            status = DebugResponseJsonString(document, item, "message", 0, result->message,
                                             sizeof result->message);
        int64_t number = 0;
        if (status == UMI_STATUS_OK)
            status = FunctionNumber(document, item, "id", INT64_MAX, &number);
        if (status == UMI_STATUS_OK)
            result->adapter_id = (uint64_t)number;
        const char *coordinates[] = {"line", "column"};
        for (size_t k = 0U; status == UMI_STATUS_OK && k < 2U; ++k)
        {
            status = FunctionNumber(document, item, coordinates[k], INT32_MAX, &number);
            int present = DebugResponseJsonField(document, item, coordinates[k]) >= 0;
            if (status == UMI_STATUS_OK && present && number == 0)
                status = UMI_STATUS_PARSE_ERROR;
            if (status == UMI_STATUS_OK)
            {
                if (k == 0U)
                    result->line = (uint32_t)number;
                else
                    result->column = (uint32_t)number;
            }
        }
        int source = DebugResponseJsonField(document, item, "source");
        if (status == UMI_STATUS_OK && source >= 0)
        {
            status = DebugResponseJsonObject(document, source);
            if (status == UMI_STATUS_OK)
                status = DebugResponseJsonString(document, source, "path", 0, result->source,
                                                 sizeof result->source);
        }
    }
    if (status == UMI_STATUS_OK)
        *out = *reply;
    free(reply);
    free(document);
    return status;
}
