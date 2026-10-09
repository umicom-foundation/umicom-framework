/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/exception_information.c
 * PURPOSE: Decode nested exception details with bounded depth and owned descriptive text.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "response_fields_internal.h"
#include "umicom/debug_runtime/exception_inspection.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus ExceptionDetailRead(const UmiLanguageRuntimeJsonDocument *document, int object,
                                     size_t parent, unsigned depth,
                                     UmiDebugExceptionInformation *result)
{
    if (depth >= UMI_DEBUG_EXCEPTION_DEPTH_LIMIT ||
        result->count >= UMI_DEBUG_EXCEPTION_DETAIL_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = DebugResponseJsonObject(document, object);
    if (status != UMI_STATUS_OK)
        return status;
    size_t index = result->count++;
    UmiDebugExceptionDetail *detail = &result->details[index];
    detail->parent = parent;
    detail->depth = depth;
#define EXCEPTION_TEXT(field, key)                                                                 \
    if (status == UMI_STATUS_OK)                                                                   \
    status = DebugResponseJsonString(document, object, key, 0, detail->field, sizeof detail->field)
    EXCEPTION_TEXT(message, "message");
    EXCEPTION_TEXT(type_name, "typeName");
    EXCEPTION_TEXT(full_type_name, "fullTypeName");
    EXCEPTION_TEXT(evaluate_name, "evaluateName");
    EXCEPTION_TEXT(stack_trace, "stackTrace");
#undef EXCEPTION_TEXT
    int children = DebugResponseJsonField(document, object, "innerException");
    if (status == UMI_STATUS_OK && children >= 0)
    {
        if (document->tokens[children].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            return UMI_STATUS_PARSE_ERROR;
        size_t count = umi_language_runtime_json_array_count(document, children);
        for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
            status = ExceptionDetailRead(document,
                                         umi_language_runtime_json_array_at(document, children, i),
                                         index, depth + 1U, result);
    }
    return status;
}
UmiStatus UmiDebugExceptionInformationDecode(const char *json, UmiDebugExceptionInformation *out)
{
    if (json == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Large stack traces belong to an operation-owned allocation. The recursion
     * above holds only indexes and pointers, and its depth is checked first. */
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    UmiDebugExceptionInformation *result = calloc(1U, sizeof *result);
    if (document == NULL || result == NULL)
    {
        free(document);
        free(result);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonResponse(document, "exceptionInfo", 0, &body);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, body, "exceptionId", 1, result->exception_id,
                                         sizeof result->exception_id);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, body, "breakMode", 1, result->break_mode,
                                         sizeof result->break_mode);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, body, "description", 0, result->description,
                                         sizeof result->description);
    if (status == UMI_STATUS_OK)
    {
        int detail = DebugResponseJsonField(document, body, "details");
        if (detail >= 0)
            status = ExceptionDetailRead(document, detail, SIZE_MAX, 0U, result);
    }
    if (status == UMI_STATUS_OK)
        *out = *result;
    free(result);
    free(document);
    return status;
}
