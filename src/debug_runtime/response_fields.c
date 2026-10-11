/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/response_fields.c
 * PURPOSE: Validate debugger response envelopes, unique keys and typed fields in one place.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "response_fields_internal.h"
#include <string.h>
int DebugResponseJsonField(const UmiLanguageRuntimeJsonDocument *document, int object,
                           const char *key)
{
    if (object < 0 || document->tokens[object].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return -1;
    size_t count = umi_language_runtime_json_object_count(document, object);
    for (size_t i = 0U; i < count; ++i)
    {
        int name_token, value_token;
        char name[256];
        if (umi_language_runtime_json_object_entry_at(document, object, i, &name_token,
                                                      &value_token) != UMI_STATUS_OK ||
            UmiLanguageRuntimeJsonText(document, name_token, name, sizeof name) != UMI_STATUS_OK)
            return -1;
        if (strcmp(name, key) == 0)
            return value_token;
    }
    return -1;
}
UmiStatus DebugResponseJsonObject(const UmiLanguageRuntimeJsonDocument *document, int object)
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
/* Typed response fields now report malformed JSON field types as parse errors. The previous direct string wrapper is retained to document its different diagnostic classification. The previous implementation is retained for engineering review. */
#if 0
UmiStatus DebugResponseJsonString(const UmiLanguageRuntimeJsonDocument *document, int object,
                                  const char *key, int required, char *out, size_t capacity)
{
    int token = DebugResponseJsonField(document, object, key);
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
UmiStatus DebugResponseJsonString(const UmiLanguageRuntimeJsonDocument *document, int object,
                                  const char *key, int required, char *out, size_t capacity)
{
    int token = DebugResponseJsonField(document, object, key);
    if (token < 0)
    {
        if (required)
            return UMI_STATUS_PARSE_ERROR;
        out[0] = '\0';
        return UMI_STATUS_OK;
    }
    /* A wrong field type is malformed peer input, not a bad application pointer.
     * Keep capacity and Unicode errors intact so callers can explain the limit. */
    if(document->tokens[token].type!=UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    return UmiLanguageRuntimeJsonText(document, token, out, capacity);
}
UmiStatus DebugResponseJsonBoolean(const UmiLanguageRuntimeJsonDocument *document, int object,
                                   const char *key, int required, int *out)
{
    int token = DebugResponseJsonField(document, object, key);
    if (token < 0)
    {
        if (required)
            return UMI_STATUS_PARSE_ERROR;
        *out = 0;
        return UMI_STATUS_OK;
    }
    return umi_language_runtime_json_bool(document, token, out);
}
UmiStatus DebugResponseJsonResponse(const UmiLanguageRuntimeJsonDocument *document,
                                    const char *command, int body_optional, int *body)
{
    UmiStatus status = DebugResponseJsonObject(document, 0);
    char actual[64], type[32];
    int success = 0;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, 0, "type", 1, type, sizeof type);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, 0, "command", 1, actual, sizeof actual);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonBoolean(document, 0, "success", 1, &success);
    if (status != UMI_STATUS_OK)
        return status;
    if (strcmp(type, "response") != 0 || strcmp(actual, command) != 0)
        return UMI_STATUS_PARSE_ERROR;
    if (!success)
        return UMI_STATUS_UNAVAILABLE;
    *body = DebugResponseJsonField(document, 0, "body");
    return *body < 0 && body_optional ? UMI_STATUS_OK : DebugResponseJsonObject(document, *body);
}

/* Keep numeric field policy shared by stack and instruction inspection. These
 * helpers write temporary caller-owned fields; public decoders publish only
 * after the complete response has been validated. */
UmiStatus DebugResponseJsonUnsigned(const UmiLanguageRuntimeJsonDocument *document, int object,
                                    const char *key, int required, uint64_t limit, uint64_t *out,
                                    int *known)
{
    int token = DebugResponseJsonField(document, object, key);
    *known = token >= 0;
    *out = 0U;
    if (token < 0)
        return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    int64_t value = 0;
    UmiStatus status = umi_language_runtime_json_int64(document, token, &value);
    if (status != UMI_STATUS_OK)
        return status;
    if (value < 0 || (uint64_t)value > limit)
        return UMI_STATUS_PARSE_ERROR;
    *out = (uint64_t)value;
    return UMI_STATUS_OK;
}
/* Source fields describe adapter data. Validate recognized fields and duplicate
 * keys while allowing future, unrecognized metadata to remain with the adapter. */
UmiStatus DebugResponseJsonSource(const UmiLanguageRuntimeJsonDocument *document, int object,
                                  UmiDebugRuntimeSource *out)
{
    UmiStatus status = DebugResponseJsonObject(document, object);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, object, "name", 0, out->name, sizeof out->name);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, object, "path", 0, out->path, sizeof out->path);
    int known = 0;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonUnsigned(document, object, "sourceReference", 0, INT32_MAX,
                                           &out->source_reference, &known);
    return status;
}
