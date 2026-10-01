/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/variable_reply.c
 * PURPOSE: Decode complete child lists and distinguish absent data from malformed or oversized data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/variable_inspection.h"
#include "umicom/language_runtime/json_text.h"
#include "umicom/language_runtime/json_document.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
/* Decode member names before comparing them: escaped spellings must not hide
 * a second value for a recognized field. Extension values remain opaque. */
static UmiStatus Member(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name, int *out)
{
    if (object < 0 || doc->tokens[object].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    *out = -1;
    size_t count = umi_language_runtime_json_object_count(doc, object);
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
static UmiStatus Text(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name,
    int required, char *out, size_t capacity)
{
    int token; UmiStatus status = Member(doc, object, name, &token);
    if (status != UMI_STATUS_OK) return status;
    if (token < 0) return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    if (doc->tokens[token].type != UMI_LANGUAGE_RUNTIME_JSON_STRING) return UMI_STATUS_PARSE_ERROR;
    return UmiLanguageRuntimeJsonText(doc, token, out, capacity);
}
static UmiStatus Number(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name,
    int required, uint32_t *out)
{
    int token; UmiStatus status = Member(doc, object, name, &token);
    if (status != UMI_STATUS_OK) return status;
    if (token < 0) return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    const UmiLanguageRuntimeJsonToken *t = &doc->tokens[token];
    if (t->type != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE || t->end <= t->start) return UMI_STATUS_PARSE_ERROR;
    if (doc->json[t->start] == '0' && t->end - t->start != 1) return UMI_STATUS_PARSE_ERROR;
    uint32_t value = 0U;
    for (int i = t->start; i < t->end; ++i) {
        unsigned char c = (unsigned char)doc->json[i];
        if (c < '0' || c > '9') return UMI_STATUS_PARSE_ERROR;
        uint32_t digit = (uint32_t)(c - '0');
        if (value > ((uint32_t)INT32_MAX - digit) / 10U) return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * 10U + digit;
    }
    *out = value; return UMI_STATUS_OK;
}
UmiStatus UmiDebugRuntimeDecodeVariableChildren(const char *json, UmiDebugVariableChildren *out)
{
    if (json == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Both buffers belong on the heap, especially on Windows GTK threads. */
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof *doc);
    UmiDebugVariableChildren *children = calloc(1, sizeof *children);
    if (doc == NULL || children == NULL) { free(doc); free(children); return UMI_STATUS_OUT_OF_MEMORY; }
    int body = -1, array = -1;
    UmiStatus status = UmiLanguageRuntimeJsonParseComplete(json, doc);
    if (status == UMI_STATUS_OK) status = Member(doc, 0, "body", &body);
    if (status == UMI_STATUS_OK) status = Member(doc, body, "variables", &array);
    if (status == UMI_STATUS_OK && (array < 0 || doc->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        children->count = umi_language_runtime_json_array_count(doc, array);
        if (children->count > UMI_DEBUG_VARIABLE_CHILDREN_CAPACITY) status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < children->count; ++i) {
        int row = umi_language_runtime_json_array_at(doc, array, i);
        UmiDebugRuntimeVariable *v = &children->items[i]; uint32_t reference = 0U;
        status = Text(doc, row, "name", 1, v->name, sizeof v->name);
        if (status == UMI_STATUS_OK) status = Text(doc, row, "value", 1, v->value, sizeof v->value);
        if (status == UMI_STATUS_OK) status = Text(doc, row, "type", 0, v->type, sizeof v->type);
        if (status == UMI_STATUS_OK) status = Text(doc, row, "evaluateName", 0, v->evaluate_name, sizeof v->evaluate_name);
        if (status == UMI_STATUS_OK) status = Text(doc, row, "memoryReference", 0, v->memory_reference, sizeof v->memory_reference);
        if (status == UMI_STATUS_OK) status = Number(doc, row, "variablesReference", 1, &reference);
        v->variables_reference = reference;
        if (status == UMI_STATUS_OK) status = Number(doc, row, "namedVariables", 0, &v->named_variables);
        if (status == UMI_STATUS_OK) status = Number(doc, row, "indexedVariables", 0, &v->indexed_variables);
    }
    if (status == UMI_STATUS_OK) *out = *children;
    free(doc); free(children); return status;
}
