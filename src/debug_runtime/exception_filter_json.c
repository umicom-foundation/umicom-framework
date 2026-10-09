/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/exception_filter_json.c
 * PURPOSE: Reject ambiguous exception metadata before it can become debugger configuration.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "exception_filter_json_internal.h"
#include "response_fields_internal.h"
#include <string.h>
/* Share strict response validation with other breakpoint operations. The earlier exception-specific implementation is retained for review; this interface now delegates to the common decoder. The previous implementation is retained for engineering review. */
#if 0
UmiStatus DebugExceptionJsonObject(const UmiLanguageRuntimeJsonDocument *document, int object)
{
    if (object < 0 || document->tokens[object].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    size_t count = umi_language_runtime_json_object_count(document, object);
    /* Duplicate keys can make independent parsers disagree. Decode keys before
     * comparing them, so an escaped spelling cannot hide a duplicate. */
    for (size_t i = 0U; i < count; ++i)
    {
        int key, value;
        char name[256];
        UmiStatus status =
            umi_language_runtime_json_object_entry_at(document, object, i, &key, &value);
        if (status == UMI_STATUS_OK)
            status = UmiLanguageRuntimeJsonText(document, key, name, sizeof name);
        if (status != UMI_STATUS_OK)
            return status;
        for (size_t j = 0U; j < i; ++j)
        {
            char previous[256];
            status = umi_language_runtime_json_object_entry_at(document, object, j, &key, &value);
            if (status == UMI_STATUS_OK)
                status = UmiLanguageRuntimeJsonText(document, key, previous, sizeof previous);
            if (status != UMI_STATUS_OK)
                return status;
            if (strcmp(name, previous) == 0)
                return UMI_STATUS_PARSE_ERROR;
        }
    }
    return UMI_STATUS_OK;
}
#endif
UmiStatus DebugExceptionJsonObject(const UmiLanguageRuntimeJsonDocument *document, int object)
{
    return DebugResponseJsonObject(document, object);
}
/* Share strict response validation with other breakpoint operations. The earlier exception-specific implementation is retained for review; this interface now delegates to the common decoder. The previous implementation is retained for engineering review. */
#if 0
UmiStatus DebugExceptionJsonString(const UmiLanguageRuntimeJsonDocument *document, int object,
                                   const char *key, int required, char *out, size_t capacity)
{
    int token = umi_language_runtime_json_object_get(document, object, key);
    if (token < 0)
    {
        if (required)
            return UMI_STATUS_PARSE_ERROR;
        out[0] = '\0';
        return UMI_STATUS_OK;
    }
    return UmiLanguageRuntimeJsonText(document, token, out, capacity);
}
#endif
UmiStatus DebugExceptionJsonString(const UmiLanguageRuntimeJsonDocument *document, int object, const char *key, int required, char *out, size_t capacity)
{
    return DebugResponseJsonString(document, object, key, required, out, capacity);
}
/* Share strict response validation with other breakpoint operations. The earlier exception-specific implementation is retained for review; this interface now delegates to the common decoder. The previous implementation is retained for engineering review. */
#if 0
UmiStatus DebugExceptionJsonBoolean(const UmiLanguageRuntimeJsonDocument *document, int object,
                                    const char *key, int required, int *out)
{
    int token = umi_language_runtime_json_object_get(document, object, key);
    if (token < 0)
    {
        if (required)
            return UMI_STATUS_PARSE_ERROR;
        *out = 0;
        return UMI_STATUS_OK;
    }
    return umi_language_runtime_json_bool(document, token, out);
}
#endif
UmiStatus DebugExceptionJsonBoolean(const UmiLanguageRuntimeJsonDocument *document, int object, const char *key, int required, int *out)
{
    return DebugResponseJsonBoolean(document, object, key, required, out);
}
/* Share strict response validation with other breakpoint operations. The earlier exception-specific implementation is retained for review; this interface now delegates to the common decoder. The previous implementation is retained for engineering review. */
#if 0
UmiStatus DebugExceptionJsonResponse(const UmiLanguageRuntimeJsonDocument *document,
                                     const char *command, int body_optional, int *body)
{
    UmiStatus status = DebugExceptionJsonObject(document, 0);
    char actual[64], type[32];
    int success = 0;
    if (status == UMI_STATUS_OK)
        status = DebugExceptionJsonString(document, 0, "type", 1, type, sizeof type);
    if (status == UMI_STATUS_OK)
        status = DebugExceptionJsonString(document, 0, "command", 1, actual, sizeof actual);
    if (status == UMI_STATUS_OK)
        status = DebugExceptionJsonBoolean(document, 0, "success", 1, &success);
    if (status != UMI_STATUS_OK)
        return status;
    if (strcmp(type, "response") != 0 || strcmp(actual, command) != 0)
        return UMI_STATUS_PARSE_ERROR;
    if (!success)
        return UMI_STATUS_UNAVAILABLE;
    *body = umi_language_runtime_json_object_get(document, 0, "body");
    return *body < 0 && body_optional ? UMI_STATUS_OK : DebugExceptionJsonObject(document, *body);
}
#endif
UmiStatus DebugExceptionJsonResponse(const UmiLanguageRuntimeJsonDocument *document, const char *command, int body_optional, int *body)
{
    return DebugResponseJsonResponse(document, command, body_optional, body);
}
