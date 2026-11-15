/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/json.c
 *
 * PURPOSE:
 *   Implement the small JSON operations needed by MCP descriptors while preserving raw nested schema objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/json.h"

#include "json_internal.h"
#include "umicom/protocol/json_writer.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Provide the find key operation used by this module and its client applications. */
static const char *find_key(const char *json, const char *key)
{
    char pattern[UMI_AI_ID_CAPACITY + 4U];
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL || key == NULL || key[0] == '\0') {
        return NULL;
    }

    length = strlen(key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 3U >= sizeof(pattern)) {
        return NULL;
    }

    pattern[0] = '"';
    (void)memcpy(pattern + 1U, key, length);
    pattern[length + 1U] = '"';
    pattern[length + 2U] = '\0';

    return strstr(json, pattern);
}

/* Provide the value begin operation used by this module and its client applications. */
static const char *value_begin(const char *json, const char *key)
{
    const char *cursor = find_key(json, key);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL) {
        return NULL;
    }

    cursor = strchr(cursor + 1, ':');
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

    return cursor;
}

/*
 * Provide the ai mcp json has key operation used by this module and its client
 * applications.
 */
int umi_ai_mcp_json_has_key(const char *json, const char *key)
{
    return find_key(json, key) != NULL;
}

/*
 * Provide the ai mcp json string operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_json_string(
    const char *json,
    const char *key,
    char *out_value,
    size_t value_capacity)
{
    const char *cursor;
    size_t length = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_value == NULL || value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_value[0] = '\0';
    cursor = value_begin(json, key);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (*cursor != '"') {
        return UMI_STATUS_PARSE_ERROR;
    }

    ++cursor;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0' && *cursor != '"') {
        char value = *cursor++;

        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '\\') {
            value = *cursor++;
            /* Apply this branch only when its contract condition is satisfied. */
            if (value == '\0') {
                return UMI_STATUS_PARSE_ERROR;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (value == 'n') value = '\n';
            else /* Apply this branch only when its contract condition is satisfied. */ if (value == 'r') value = '\r';
            else /* Apply this branch only when its contract condition is satisfied. */ if (value == 't') value = '\t';
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length + 1U >= value_capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_value[length++] = value;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (*cursor != '"') {
        return UMI_STATUS_PARSE_ERROR;
    }

    out_value[length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the ai mcp json raw operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_value == NULL || value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_value[0] = '\0';
    cursor = value_begin(json, key);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    begin = cursor;

    /* Apply this branch only when its contract condition is satisfied. */
    if (*cursor == '{' || *cursor == '[') {
        char open = *cursor;
        char close = open == '{' ? '}' : ']';

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
            } else /* Apply this branch only when its contract condition is satisfied. */ if (current == open) {
                ++depth;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (current == close) {
                --depth;
            }

            ++cursor;
        } /* Continue only while work remains available; the loop body advances the state on each pass. */ while (depth > 0);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '"') {
        ++cursor;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*cursor != '\0') {
            /* Apply this branch only when its contract condition is satisfied. */
            if (*cursor == '\\' && cursor[1] != '\0') {
                cursor += 2;
                continue;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (*cursor == '"') {
                ++cursor;
                break;
            }
            ++cursor;
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*cursor != '\0' &&
               *cursor != ',' &&
               *cursor != '}' &&
               *cursor != ']') {
            ++cursor;
        }
    }

    length = (size_t)(cursor - begin);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (length > 0U &&
           isspace((unsigned char)begin[length - 1U])) {
        --length;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > value_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(out_value, begin, length);
    }
    out_value[length] = '\0';

    return UMI_STATUS_OK;
}

/*
 * Provide the ai mcp json escape string operation used by this module and its client
 * applications.
 */
/* The protocol JSON writer owns Unicode validation and all control escaping; a private candidate preserves caller output and supports overlapping input. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_json_escape_string(
    const char *value,
    char *out_json_string,
    size_t output_capacity)
{
    size_t output_length = 0U;
    const char *cursor;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        out_json_string == NULL ||
        output_capacity < 3U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_json_string[output_length++] = '"';

    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = value; *cursor != '\0'; ++cursor) {
        char current = *cursor;
        const char *escape = NULL;

        /* Apply this branch only when its contract condition is satisfied. */
        if (current == '"' || current == '\\') {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (output_length + 2U >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_json_string[output_length++] = '\\';
            out_json_string[output_length++] = current;
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (current == '\n') escape = "\\n";
        else /* Apply this branch only when its contract condition is satisfied. */ if (current == '\r') escape = "\\r";
        else /* Apply this branch only when its contract condition is satisfied. */ if (current == '\t') escape = "\\t";

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (escape != NULL) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (output_length + 2U >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_json_string[output_length++] = escape[0];
            out_json_string[output_length++] = escape[1];
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (output_length + 1U >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_json_string[output_length++] = current;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (output_length + 2U > output_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    out_json_string[output_length++] = '"';
    out_json_string[output_length] = '\0';

    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_mcp_json_escape_string(const char *value, char *out_json_string, size_t output_capacity)
{
    if (value == NULL || out_json_string == NULL || output_capacity < 3U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_AI_MCP_TEXT_CAPACITY && value[length] != '\0') ++length;
    if (length == UMI_AI_MCP_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Every bounded input byte needs at most six JSON bytes, plus quotes and
     * termination. Staging supports overlap and preserves output on refusal. */
    size_t capacity = length * 6U + 3U;
    if (capacity > output_capacity) capacity = output_capacity;
    char *candidate = malloc(capacity);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, candidate, capacity);
    UmiStatus status = UmiJsonWriterString(&writer, value);
    if (status == UMI_STATUS_OK) memmove(out_json_string, candidate, writer.length + 1U);
    free(candidate);
    return status;
}

/* All MCP descriptor paths share complete bounded parsing. The tree owns its
 * source bytes so final output may alias the original caller input safely. */
UmiStatus umi_ai_mcp_json_object_parse(const char *json, UmiJsonTree **out_tree)
{
    if (json == NULL || out_tree == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_AI_MCP_TEXT_CAPACITY && json[length] != '\0') ++length;
    if (length == UMI_AI_MCP_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiJsonTree *tree = NULL;
    const UmiJsonTreeLimits limits = {UMI_AI_MCP_TEXT_CAPACITY - 1U, UMI_AI_MCP_TEXT_CAPACITY, 128U};
    UmiStatus status = UmiJsonTreeCreate(json, length, &limits, NULL, &tree);
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        UmiJsonTreeDestroy(tree); return UMI_STATUS_PARSE_ERROR;
    }
    *out_tree = tree;
    return UMI_STATUS_OK;
}
