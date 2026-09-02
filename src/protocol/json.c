/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/json.c
 *
 * PURPOSE:
 *   Implement bounded JSON helpers sufficient for controlled JSON-RPC protocol envelopes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/json.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the find field value operation used by this module and its client applications. */
static const char *find_field_value(const char *json, const char *field)
{
    char pattern[512];
    const char *match;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL || field == NULL) {
        return NULL;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (snprintf(pattern, sizeof(pattern), "\"%s\"", field) < 0) {
        return NULL;
    }
    match = strstr(json, pattern);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (match == NULL) {
        return NULL;
    }
    match += strlen(pattern);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*match != '\0' && isspace((unsigned char)*match)) {
        match += 1;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (*match != ':') {
        return NULL;
    }
    match += 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*match != '\0' && isspace((unsigned char)*match)) {
        match += 1;
    }
    return match;
}

/* Provide the json escape operation used by this module and its client applications. */
UmiStatus umi_json_escape(const char *text,
                          char *out_text,
                          size_t capacity)
{
    size_t used = 0U;
    const unsigned char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = (const unsigned char *)text; *cursor != '\0'; ++cursor) {
        const char *replacement = NULL;
        char unicode[7];
        size_t length;
        /* Select the behaviour associated with the requested command or state value. */
        switch (*cursor) {
            case '"': replacement = "\\\""; break;
            case '\\': replacement = "\\\\"; break;
            case '\b': replacement = "\\b"; break;
            case '\f': replacement = "\\f"; break;
            case '\n': replacement = "\\n"; break;
            case '\r': replacement = "\\r"; break;
            case '\t': replacement = "\\t"; break;
            default:
                /* Apply this branch only when its contract condition is satisfied. */
                if (*cursor < 0x20U) {
                    (void)snprintf(unicode,
                                   sizeof(unicode),
                                   "\\u%04x",
                                   (unsigned)*cursor);
                    replacement = unicode;
                }
                break;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (replacement != NULL) {
            length = strlen(replacement);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (used + length + 1U > capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            (void)memcpy(out_text + used, replacement, length);
            used += length;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (used + 2U > capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_text[used++] = (char)*cursor;
        }
    }
    out_text[used] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the json get string operation used by this module and its client applications. */
UmiStatus umi_json_get_string(const char *json,
                              const char *field,
                              char *out_text,
                              size_t capacity)
{
    const char *value = find_field_value(json, field);
    size_t used = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_text == NULL || capacity == 0U) {
        return value == NULL ? UMI_STATUS_NOT_FOUND
                             : UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (*value != '"') {
        return UMI_STATUS_PARSE_ERROR;
    }
    value += 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*value != '\0' && *value != '"') {
        char character = *value++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (character == '\\') {
            /* Select the behaviour associated with the requested command or state value. */
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
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (used + 2U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_text[used++] = character;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (*value != '"') {
        return UMI_STATUS_PARSE_ERROR;
    }
    out_text[used] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the json get integer operation used by this module and its client applications. */
UmiStatus umi_json_get_integer(const char *json,
                               const char *field,
                               int64_t *out_value)
{
    const char *value = find_field_value(json, field);
    char *end = NULL;
    long long parsed;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_value == NULL) {
        return value == NULL ? UMI_STATUS_NOT_FOUND
                             : UMI_STATUS_INVALID_ARGUMENT;
    }
    parsed = strtoll(value, &end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (end == value) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_value = (int64_t)parsed;
    return UMI_STATUS_OK;
}

/* Provide the json get boolean operation used by this module and its client applications. */
UmiStatus umi_json_get_boolean(const char *json,
                               const char *field,
                               int *out_value)
{
    const char *value = find_field_value(json, field);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_value == NULL) {
        return value == NULL ? UMI_STATUS_NOT_FOUND
                             : UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(value, "true", 4U) == 0) {
        *out_value = 1;
        return UMI_STATUS_OK;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(value, "false", 5U) == 0) {
        *out_value = 0;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}

/* Provide the json get object operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_json == NULL || capacity == 0U) {
        return value == NULL ? UMI_STATUS_NOT_FOUND
                             : UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (*value != '{' && *value != '[') {
        return UMI_STATUS_PARSE_ERROR;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = value; *cursor != '\0'; ++cursor) {
        char character = *cursor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (in_string) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (escaped) {
                escaped = 0;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (character == '\\') {
                escaped = 1;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (character == '"') {
                in_string = 0;
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (character == '"') {
            in_string = 1;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (character == '{' || character == '[') {
            depth += 1U;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (character == '}' || character == ']') {
            /* Apply this branch only when its contract condition is satisfied. */
            if (depth == 0U) {
                return UMI_STATUS_PARSE_ERROR;
            }
            depth -= 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (depth == 0U) {
                length = (size_t)(cursor - value) + 1U;
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Provide the json is object operation used by this module and its client applications. */
int umi_json_is_object(const char *json)
{
    const char *cursor = json;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL) {
        return 0;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0' && isspace((unsigned char)*cursor)) {
        cursor += 1;
    }
    return *cursor == '{';
}
