/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/json_document.c
 * PURPOSE: Bound nesting before recursion and validate tokens that a field decoder would otherwise ignore.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_document.h"
#include "umicom/language_runtime/json_text.h"
#include <stdlib.h>
#include <string.h>
static int Digit(char c) { return c >= '0' && c <= '9'; }
static int Primitive(const char *s, size_t n)
{
    if ((n == 4U && (memcmp(s, "true", 4U) == 0 || memcmp(s, "null", 4U) == 0)) ||
        (n == 5U && memcmp(s, "false", 5U) == 0)) return 1;
    size_t i = 0U;
    if (i < n && s[i] == '-') ++i;
    if (i == n) return 0;
    if (s[i] == '0') ++i;
    else {
        if (s[i] < '1' || s[i] > '9') return 0;
        do { ++i; } while (i < n && Digit(s[i]));
    }
    if (i < n && s[i] == '.') {
        ++i; size_t first = i;
        while (i < n && Digit(s[i])) ++i;
        if (i == first) return 0;
    }
    if (i < n && (s[i] == 'e' || s[i] == 'E')) {
        ++i; if (i < n && (s[i] == '+' || s[i] == '-')) ++i;
        size_t first = i;
        while (i < n && Digit(s[i])) ++i;
        if (i == first) return 0;
    }
    return i == n;
}
UmiStatus UmiLanguageRuntimeJsonParseComplete(const char *json, UmiLanguageRuntimeJsonDocument *out)
{
    if (json == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_LANGUAGE_RUNTIME_JSON_CAPACITY && json[length] != '\0') ++length;
    if (length >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t depth = 0U; int string = 0, escape = 0;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = (unsigned char)json[i];
        if (string) {
            if (escape) escape = 0;
            else if (c == '\\') escape = 1;
            else if (c == '"') string = 0;
        } else {
            if (c < 0x20U && c != '\t' && c != '\r' && c != '\n') return UMI_STATUS_PARSE_ERROR;
            if (c == '"') string = 1;
            else if (c == '[' || c == '{') {
                if (++depth > UMI_LANGUAGE_RUNTIME_JSON_MAX_DEPTH) return UMI_STATUS_CAPACITY_EXCEEDED;
            } else if (c == ']' || c == '}') {
                if (depth == 0U) return UMI_STATUS_PARSE_ERROR;
                --depth;
            }
        }
    }
    if (string || depth != 0U) return UMI_STATUS_PARSE_ERROR;
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof *doc);
    char *text = malloc(length + 1U);
    if (doc == NULL || text == NULL) { free(doc); free(text); return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status = umi_language_runtime_json_parse(json, doc);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < doc->token_count; ++i) {
        const UmiLanguageRuntimeJsonToken *token = &doc->tokens[i];
        if (token->type == UMI_LANGUAGE_RUNTIME_JSON_STRING)
            status = UmiLanguageRuntimeJsonText(doc, (int)i, text, length + 1U);
        else if (token->type == UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE &&
            !Primitive(json + token->start, (size_t)(token->end - token->start))) status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK) *out = *doc;
    free(doc); free(text); return status;
}
