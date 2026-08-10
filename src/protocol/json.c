/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/json.c
 *
 * PURPOSE:
 *   Implement bounded JSON helpers sufficient for controlled JSON-RPC protocol envelopes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/json.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *find_field_value(const char *json, const char *field)
{
    char pattern[512];
    const char *match;
    if (json == NULL || field == NULL) {
        return NULL;
    }
    if (snprintf(pattern, sizeof(pattern), "\"%s\"", field) < 0) {
        return NULL;
    }
    match = strstr(json, pattern);
    if (match == NULL) {
        return NULL;
    }
    match += strlen(pattern);
    while (*match != '\0' && isspace((unsigned char)*match)) {
        match += 1;
    }
    if (*match != ':') {
        return NULL;
    }
    match += 1;
    while (*match != '\0' && isspace((unsigned char)*match)) {
        match += 1;
    }
    return match;
}

UmiStatus umi_json_escape(const char *text,
                          char *out_text,
                          size_t capacity)
{
    size_t used = 0U;
    const unsigned char *cursor;
    if (text == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (cursor = (const unsigned char *)text; *cursor != '\0'; ++cursor) {
        const char *replacement = NULL;
        char unicode[7];
        size_t length;
        switch (*cursor) {
            case '"': replacement = "\\\""; break;
            case '\\': replacement = "\\\\"; break;
            case '\b': replacement = "\\b"; break;
            case '\f': replacement = "\\f"; break;
            case '\n': replacement = "\\n"; break;
            case '\r': replacement = "\\r"; break;
            case '\t': replacement = "\\t"; break;
            default:
                if (*cursor < 0x20U) {
                    (void)snprintf(unicode,
                                   sizeof(unicode),
                                   "\\u%04x",
                                   (unsigned)*cursor);
                    replacement = unicode;
                }
                break;
        }
        if (replacement != NULL) {
            length = strlen(replacement);
            if (used + length + 1U > capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            (void)memcpy(out_text + used, replacement, length);
            used += length;
        } else {
            if (used + 2U > capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_text[used++] = (char)*cursor;
        }
    }
    out_text[used] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_json_get_string(const char *json,
                              const char *field,
                              char *out_text,
                              size_t capacity)
{
    const char *value = find_field_value(json, field);
    size_t used = 0U;
    if (value == NULL || out_text == NULL || capacity == 0U) {
        return value == NULL ? UMI_STATUS_NOT_FOUND
                             : UMI_STATUS_INVALID_ARGUMENT;
    }
    if (*value != '"') {
        return UMI_STATUS_PARSE_ERROR;
    }
    value += 1;
    while (*value != '\0' && *value != '"') {
        char character = *value++;
        if (character == '\\') {
            switch (*value) {
                case '"': character = '"'; break;
                case '\\': character = '\\'; break;
                case '/': character = '/'; break;
                case 'b': character = '\b'; break;
                case 'f': character = '\f'; break;
                case 'n': character = '\n'; break;
                case 'r': character = '\r'; break;
                case 't': character = '\t'; break;
                default: return UMI_STATUS_PARSE_ERROR;
            }
            value += 1;
        }
        if (used + 2U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_text[used++] = character;
    }
    if (*value != '"') {
        return UMI_STATUS_PARSE_ERROR;
    }
    out_text[used] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_json_get_integer(const char *json,
                               const char *field,
                               int64_t *out_value)
{
    const char *value = find_field_value(json, field);
    char *end = NULL;
    long long parsed;
    if (value == NULL || out_value == NULL) {
        return value == NULL ? UMI_STATUS_NOT_FOUND
                             : UMI_STATUS_INVALID_ARGUMENT;
    }
    parsed = strtoll(value, &end, 10);
    if (end == value) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_value = (int64_t)parsed;
    return UMI_STATUS_OK;
}

UmiStatus umi_json_get_boolean(const char *json,
                               const char *field,
                               int *out_value)
{
    const char *value = find_field_value(json, field);
    if (value == NULL || out_value == NULL) {
        return value == NULL ? UMI_STATUS_NOT_FOUND
                             : UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strncmp(value, "true", 4U) == 0) {
        *out_value = 1;
        return UMI_STATUS_OK;
    }
    if (strncmp(value, "false", 5U) == 0) {
        *out_value = 0;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}

UmiStatus umi_json_get_object(const char *json,
                              const char *field,
                              char *out_json,
                              size_t capacity)
{
    const char *value = find_field_value(json, field);
    const char *cursor;
    size_t depth = 0U;
    int in_string = 0;
    int escaped = 0;
    size_t length;
    if (value == NULL || out_json == NULL || capacity == 0U) {
        return value == NULL ? UMI_STATUS_NOT_FOUND
                             : UMI_STATUS_INVALID_ARGUMENT;
    }
    if (*value != '{' && *value != '[') {
        return UMI_STATUS_PARSE_ERROR;
    }
    for (cursor = value; *cursor != '\0'; ++cursor) {
        char character = *cursor;
        if (in_string) {
            if (escaped) {
                escaped = 0;
            } else if (character == '\\') {
                escaped = 1;
            } else if (character == '"') {
                in_string = 0;
            }
        } else if (character == '"') {
            in_string = 1;
        } else if (character == '{' || character == '[') {
            depth += 1U;
        } else if (character == '}' || character == ']') {
            if (depth == 0U) {
                return UMI_STATUS_PARSE_ERROR;
            }
            depth -= 1U;
            if (depth == 0U) {
                length = (size_t)(cursor - value) + 1U;
                if (length + 1U > capacity) {
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }
                (void)memcpy(out_json, value, length);
                out_json[length] = '\0';
                return UMI_STATUS_OK;
            }
        }
    }
    return UMI_STATUS_PARSE_ERROR;
}

int umi_json_is_object(const char *json)
{
    const char *cursor = json;
    if (cursor == NULL) {
        return 0;
    }
    while (*cursor != '\0' && isspace((unsigned char)*cursor)) {
        cursor += 1;
    }
    return *cursor == '{';
}
