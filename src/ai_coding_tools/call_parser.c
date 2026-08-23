/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/call_parser.c
 *
 * PURPOSE:
 *   Implement strict parsing of one provider-emitted coding tool call.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/call_parser.h"

#include <string.h>

#define CALL_HEADER "UMICOM-TOOL-CALL/1"

static int line_equals(const char *line, size_t length, const char *value)
{
    const size_t value_length = strlen(value);
    return length == value_length &&
        strncmp(line, value, length) == 0;
}

UmiStatus umi_ai_coding_tool_call_parse(
    const char *text,
    uint64_t call_id,
    UmiAiCodingToolCall *out_call)
{
    const char *cursor;
    size_t argument_length = 0U;
    int in_arguments = 0;
    int header_seen = 0;
    int arguments_done = 0;

    if (text == NULL || out_call == NULL || call_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_call, 0, sizeof(*out_call));
    out_call->call_id = call_id;
    out_call->state = UMI_AI_CODING_TOOL_CALL_PENDING;
    out_call->status = UMI_STATUS_OK;

    cursor = text;

    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length =
            end != NULL ? (size_t)(end - cursor) : strlen(cursor);

        if (length > 0U && cursor[length - 1U] == '\r') --length;

        if (!header_seen) {
            if (!line_equals(cursor, length, CALL_HEADER)) {
                return UMI_STATUS_PARSE_ERROR;
            }
            header_seen = 1;
        } else if (in_arguments) {
            if (line_equals(cursor, length, "ARGUMENTS-END")) {
                in_arguments = 0;
                arguments_done = 1;
            } else {
                if (argument_length + length + 2U >
                    sizeof(out_call->arguments_json)) {
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }

                if (length > 0U) {
                    (void)memcpy(
                        out_call->arguments_json + argument_length,
                        cursor,
                        length);
                    argument_length += length;
                }

                out_call->arguments_json[argument_length++] = '\n';
                out_call->arguments_json[argument_length] = '\0';
            }
        } else if (length > 5U &&
                   strncmp(cursor, "TOOL|", 5U) == 0) {
            const size_t tool_length = length - 5U;

            if (tool_length == 0U ||
                tool_length >= sizeof(out_call->tool_id)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }

            (void)memcpy(out_call->tool_id, cursor + 5U, tool_length);
            out_call->tool_id[tool_length] = '\0';
        } else if (length > 9U &&
                   strncmp(cursor, "APPROVED|", 9U) == 0) {
            if (length == 10U && cursor[9] == '1') {
                out_call->approved = 1;
            } else if (length == 10U && cursor[9] == '0') {
                out_call->approved = 0;
            } else {
                return UMI_STATUS_PARSE_ERROR;
            }
        } else if (line_equals(cursor, length, "ARGUMENTS-BEGIN")) {
            if (arguments_done) return UMI_STATUS_PARSE_ERROR;
            in_arguments = 1;
        }

        if (end == NULL) break;
        cursor = end + 1;
    }

    if (!header_seen || in_arguments ||
        out_call->tool_id[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }

    if (argument_length > 0U &&
        out_call->arguments_json[argument_length - 1U] == '\n') {
        out_call->arguments_json[argument_length - 1U] = '\0';
    }

    if (out_call->arguments_json[0] == '\0') {
        (void)strcpy(out_call->arguments_json, "{}");
    }

    return UMI_STATUS_OK;
}
