/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/exception_filter_acknowledgement.c
 * PURPOSE: Separate accepted configuration from individual adapter verification.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "exception_filter_json_internal.h"
#include "response_fields_internal.h"
#include <stdlib.h>
UmiStatus UmiDebugExceptionAcknowledgementDecode(const char *json, size_t selected_count,
                                                 UmiDebugExceptionAcknowledgement *out)
{
    if (json == NULL || out == NULL || selected_count > UMI_DEBUG_EXCEPTION_FILTER_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    UmiDebugExceptionAcknowledgement *reply = calloc(1U, sizeof *reply);
    if (document == NULL || reply == NULL)
    {
        free(document);
        free(reply);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    reply->count = selected_count;
    for (size_t i = 0U; i < UMI_DEBUG_EXCEPTION_FILTER_LIMIT; ++i)
        reply->verified[i] = -1;
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1, array = -1;
    if (status == UMI_STATUS_OK)
        status = DebugExceptionJsonResponse(document, "setExceptionBreakpoints", 1, &body);
    if (status == UMI_STATUS_OK && body >= 0)
        array = DebugResponseJsonField(document, body, "breakpoints");
    if (status == UMI_STATUS_OK && array >= 0)
    {
        if (document->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
            umi_language_runtime_json_array_count(document, array) != selected_count)
            status = UMI_STATUS_PARSE_ERROR;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < selected_count; ++i)
        {
            int item = umi_language_runtime_json_array_at(document, array, i);
            status = DebugExceptionJsonObject(document, item);
            if (status == UMI_STATUS_OK)
                status =
                    DebugExceptionJsonBoolean(document, item, "verified", 1, &reply->verified[i]);
            if (status == UMI_STATUS_OK)
                status = DebugExceptionJsonString(document, item, "message", 0, reply->message[i],
                                                  sizeof reply->message[i]);
        }
    }
    if (status == UMI_STATUS_OK)
        *out = *reply;
    free(reply);
    free(document);
    return status;
}
