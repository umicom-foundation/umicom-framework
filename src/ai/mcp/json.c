/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/json.c
 *
 * PURPOSE:
 *   Implement the small JSON operations needed by MCP descriptors while preserving raw nested schema objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/json.h"

#include <ctype.h>
#include <string.h>

static const char *find_key(const char *json, const char *key)
{
    char pattern[UMI_AI_ID_CAPACITY + 4U];
    size_t length;

    if (json == NULL || key == NULL || key[0] == '\0') {
        return NULL;
    }

    length = strlen(key);
    if (length + 3U >= sizeof(pattern)) {
        return NULL;
    }

    pattern[0] = '"';
    (void)memcpy(pattern + 1U, key, length);
    pattern[length + 1U] = '"';
    pattern[length + 2U] = '\0';

    return strstr(json, pattern);
}

static const char *value_begin(const char *json, const char *key)
{
    const char *cursor = find_key(json, key);

    if (cursor == NULL) {
        return NULL;
    }

    cursor = strchr(cursor + 1, ':');
    if (cursor == NULL) {
        return NULL;
    }

    ++cursor;
    while (*cursor != '\0' &&
           isspace((unsigned char)*cursor)) {
        ++cursor;
    }

    return cursor;
}

int umi_ai_mcp_json_has_key(const char *json, const char *key)
{
    return find_key(json, key) != NULL;
}

UmiStatus umi_ai_mcp_json_string(
    const char *json,
    const char *key,
    char *out_value,
    size_t value_capacity)
{
    const char *cursor;
    size_t length = 0U;

    if (out_value == NULL || value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_value[0] = '\0';
    cursor = value_begin(json, key);

    if (cursor == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (*cursor != '"') {
        return UMI_STATUS_PARSE_ERROR;
    }

    ++cursor;
    while (*cursor != '\0' && *cursor != '"') {
        char value = *cursor++;

        if (value == '\\') {
            value = *cursor++;
            if (value == '\0') {
                return UMI_STATUS_PARSE_ERROR;
            }
            if (value == 'n') value = '\n';
            else if (value == 'r') value = '\r';
            else if (value == 't') value = '\t';
        }

        if (length + 1U >= value_capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_value[length++] = value;
    }

    if (*cursor != '"') {
        return UMI_STATUS_PARSE_ERROR;
    }

    out_value[length] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_json_raw(
    const char *json,
    const char *key,
    char *out_value,
    size_t value_capacity)
{
    const char *cursor;
    const char *begin;
    size_t length;
    int depth = 0;
    int in_string = 0;
    int escaped = 0;

    if (out_value == NULL || value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_value[0] = '\0';
    cursor = value_begin(json, key);
    if (cursor == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    begin = cursor;

    if (*cursor == '{' || *cursor == '[') {
        char open = *cursor;
        char close = open == '{' ? '}' : ']';

        do {
            char current = *cursor;
            if (current == '\0') {
                return UMI_STATUS_PARSE_ERROR;
            }

            if (in_string) {
                if (escaped) {
                    escaped = 0;
                } else if (current == '\\') {
                    escaped = 1;
                } else if (current == '"') {
                    in_string = 0;
                }
            } else if (current == '"') {
                in_string = 1;
            } else if (current == open) {
                ++depth;
            } else if (current == close) {
                --depth;
            }

            ++cursor;
        } while (depth > 0);
    } else if (*cursor == '"') {
        ++cursor;
        while (*cursor != '\0') {
            if (*cursor == '\\' && cursor[1] != '\0') {
                cursor += 2;
                continue;
            }
            if (*cursor == '"') {
                ++cursor;
                break;
            }
            ++cursor;
        }
    } else {
        while (*cursor != '\0' &&
               *cursor != ',' &&
               *cursor != '}' &&
               *cursor != ']') {
            ++cursor;
        }
    }

    length = (size_t)(cursor - begin);
    while (length > 0U &&
           isspace((unsigned char)begin[length - 1U])) {
        --length;
    }

    if (length + 1U > value_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    if (length > 0U) {
        (void)memcpy(out_value, begin, length);
    }
    out_value[length] = '\0';

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_json_escape_string(
    const char *value,
    char *out_json_string,
    size_t output_capacity)
{
    size_t output_length = 0U;
    const char *cursor;

    if (value == NULL ||
        out_json_string == NULL ||
        output_capacity < 3U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_json_string[output_length++] = '"';

    for (cursor = value; *cursor != '\0'; ++cursor) {
        char current = *cursor;
        const char *escape = NULL;

        if (current == '"' || current == '\\') {
            if (output_length + 2U >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_json_string[output_length++] = '\\';
            out_json_string[output_length++] = current;
            continue;
        }

        if (current == '\n') escape = "\\n";
        else if (current == '\r') escape = "\\r";
        else if (current == '\t') escape = "\\t";

        if (escape != NULL) {
            if (output_length + 2U >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_json_string[output_length++] = escape[0];
            out_json_string[output_length++] = escape[1];
        } else {
            if (output_length + 1U >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_json_string[output_length++] = current;
        }
    }

    if (output_length + 2U > output_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    out_json_string[output_length++] = '"';
    out_json_string[output_length] = '\0';

    return UMI_STATUS_OK;
}
