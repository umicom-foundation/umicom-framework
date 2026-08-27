/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/json_array.c
 *
 * PURPOSE:
 *   Implement nested-object-safe iteration for MCP list results without becoming a general-purpose JSON parser.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/json_array.h"

#include <ctype.h>
#include <string.h>

static const char *array_begin(
    const char *json,
    const char *array_key)
{
    char pattern[UMI_AI_ID_CAPACITY + 4U];
    const char *cursor;
    size_t key_length;

    if (json == NULL ||
        array_key == NULL ||
        array_key[0] == '\0') {
        return NULL;
    }

    key_length = strlen(array_key);
    if (key_length + 3U >= sizeof(pattern)) {
        return NULL;
    }

    pattern[0] = '"';
    (void)memcpy(pattern + 1U, array_key, key_length);
    pattern[key_length + 1U] = '"';
    pattern[key_length + 2U] = '\0';

    cursor = strstr(json, pattern);
    if (cursor == NULL) {
        return NULL;
    }

    cursor = strchr(cursor + key_length + 2U, ':');
    if (cursor == NULL) {
        return NULL;
    }

    ++cursor;
    while (*cursor != '\0' &&
           isspace((unsigned char)*cursor)) {
        ++cursor;
    }

    return *cursor == '[' ? cursor + 1 : NULL;
}

UmiStatus umi_ai_mcp_json_array_visit_objects(
    const char *json,
    const char *array_key,
    UmiAiMcpJsonObjectVisitor visitor,
    void *user_data,
    size_t *out_count)
{
    const char *cursor;
    size_t count = 0U;

    if (visitor == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_count = 0U;
    cursor = array_begin(json, array_key);
    if (cursor == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    while (*cursor != '\0') {
        const char *begin;
        const char *end;
        char object[UMI_AI_MCP_TEXT_CAPACITY];
        size_t length;
        int depth = 0;
        int in_string = 0;
        int escaped = 0;

        while (*cursor != '\0' &&
               (isspace((unsigned char)*cursor) ||
                *cursor == ',')) {
            ++cursor;
        }

        if (*cursor == ']') {
            *out_count = count;
            return UMI_STATUS_OK;
        }

        if (*cursor != '{') {
            return UMI_STATUS_PARSE_ERROR;
        }

        begin = cursor;
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
            } else if (current == '{') {
                ++depth;
            } else if (current == '}') {
                --depth;
            }

            ++cursor;
        } while (depth > 0);

        end = cursor;
        length = (size_t)(end - begin);

        if (length + 1U > sizeof(object)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(object, begin, length);
        object[length] = '\0';

        {
            UmiStatus status = visitor(object, user_data);
            if (status != UMI_STATUS_OK) {
                return status;
            }
        }

        count += 1U;
    }

    return UMI_STATUS_PARSE_ERROR;
}
