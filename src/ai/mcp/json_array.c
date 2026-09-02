/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/json_array.c
 *
 * PURPOSE:
 *   Implement nested-object-safe iteration for MCP list results without becoming a general-purpose JSON parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/json_array.h"

#include <ctype.h>
#include <string.h>

/* Provide the array begin operation used by this module and its client applications. */
static const char *array_begin(
    const char *json,
    const char *array_key)
{
    char pattern[UMI_AI_ID_CAPACITY + 4U];
    const char *cursor;
    size_t key_length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL ||
        array_key == NULL ||
        array_key[0] == '\0') {
        return NULL;
    }

    key_length = strlen(array_key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (key_length + 3U >= sizeof(pattern)) {
        return NULL;
    }

    pattern[0] = '"';
    (void)memcpy(pattern + 1U, array_key, key_length);
    pattern[key_length + 1U] = '"';
    pattern[key_length + 2U] = '\0';

    cursor = strstr(json, pattern);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL) {
        return NULL;
    }

    cursor = strchr(cursor + key_length + 2U, ':');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL) {
        return NULL;
    }

    ++cursor;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0' &&
           isspace((unsigned char)*cursor)) {
        ++cursor;
    }

    return *cursor == '[' ? cursor + 1 : NULL;
}

/*
 * Provide the ai mcp json array visit objects operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_json_array_visit_objects(
    const char *json,
    const char *array_key,
    UmiAiMcpJsonObjectVisitor visitor,
    void *user_data,
    size_t *out_count)
{
    const char *cursor;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (visitor == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_count = 0U;
    cursor = array_begin(json, array_key);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *begin;
        const char *end;
        char object[UMI_AI_MCP_TEXT_CAPACITY];
        size_t length;
        int depth = 0;
        int in_string = 0;
        int escaped = 0;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*cursor != '\0' &&
               (isspace((unsigned char)*cursor) ||
                *cursor == ',')) {
            ++cursor;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == ']') {
            *out_count = count;
            return UMI_STATUS_OK;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor != '{') {
            return UMI_STATUS_PARSE_ERROR;
        }

        begin = cursor;
        do {
            char current = *cursor;

            /* Apply this branch only when its contract condition is satisfied. */
            if (current == '\0') {
                return UMI_STATUS_PARSE_ERROR;
            }

            /* Apply this branch only when its contract condition is satisfied. */
            if (in_string) {
                /* Apply this branch only when its contract condition is satisfied. */
                if (escaped) {
                    escaped = 0;
                } else /* Apply this branch only when its contract condition is satisfied. */ if (current == '\\') {
                    escaped = 1;
                } else /* Apply this branch only when its contract condition is satisfied. */ if (current == '"') {
                    in_string = 0;
                }
            } else /* Apply this branch only when its contract condition is satisfied. */ if (current == '"') {
                in_string = 1;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (current == '{') {
                ++depth;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (current == '}') {
                --depth;
            }

            ++cursor;
        } /* Continue only while work remains available; the loop body advances the state on each pass. */ while (depth > 0);

        end = cursor;
        length = (size_t)(end - begin);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length + 1U > sizeof(object)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(object, begin, length);
        object[length] = '\0';

        {
            UmiStatus status = visitor(object, user_data);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
        }

        count += 1U;
    }

    return UMI_STATUS_PARSE_ERROR;
}
