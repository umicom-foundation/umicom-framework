/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/watch_reply.c
 * PURPOSE: Require complete unambiguous watch result fields before replacing a captured value.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/watch_evaluation.h"
#include "umicom/language_runtime/json_text.h"
#include <stdlib.h>
#include <string.h>
/* Count decoded member names so escaped spellings cannot hide a duplicate.
 * Unknown bounded extension names are accepted, never interpreted. */
static UmiStatus Member(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name, int *out)
{
    if (object < 0 || doc->tokens[object].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    *out = -1; size_t count = umi_language_runtime_json_object_count(doc, object);
    for (size_t i = 0U; i < count; ++i) {
        int key, value; char text[256];
        UmiStatus status = umi_language_runtime_json_object_entry_at(doc, object, i, &key, &value);
        if (status == UMI_STATUS_OK) status = UmiLanguageRuntimeJsonText(doc, key, text, sizeof text);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(text, name) == 0) {
            if (*out >= 0) return UMI_STATUS_PARSE_ERROR;
            *out = value;
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugRuntimeDecodeWatchValue(const char *json, UmiDebugWatchValue *out)
{
    if (json == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* The bounded document has many tokens; do not consume a Windows thread's
     * small stack while the native workbench is also rendering its rows. */
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof *doc);
    if (doc == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiDebugWatchValue value = {0}; int body = -1, result = -1, type = -1;
    UmiStatus status = umi_language_runtime_json_parse(json, doc);
    if (status == UMI_STATUS_OK) status = Member(doc, 0, "body", &body);
    if (status == UMI_STATUS_OK) status = Member(doc, body, "result", &result);
    if (status == UMI_STATUS_OK && (result < 0 || doc->tokens[result].type != UMI_LANGUAGE_RUNTIME_JSON_STRING))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiLanguageRuntimeJsonText(doc, result, value.value, sizeof value.value);
    if (status == UMI_STATUS_OK) status = Member(doc, body, "type", &type);
    if (status == UMI_STATUS_OK && type >= 0) {
        if (doc->tokens[type].type != UMI_LANGUAGE_RUNTIME_JSON_STRING) status = UMI_STATUS_PARSE_ERROR;
        else status = UmiLanguageRuntimeJsonText(doc, type, value.type, sizeof value.type);
    }
    free(doc);
    if (status == UMI_STATUS_OK) *out = value;
    return status;
}
