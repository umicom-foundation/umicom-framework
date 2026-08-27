/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/helix/response_parser.c
 *
 * PURPOSE:
 *   Parse six bounded response fields and reject malformed or ambiguous model output.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/helix/response_parser.h"
#include "umicom/ai/helix/protocol.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus extract_field(const char *text,
                               const char *key,
                               int strict,
                               char *output,
                               size_t output_capacity)
{
    const size_t key_length = strlen(key);
    const char *cursor = text;
    size_t matches = 0U;
    if (text == NULL || key == NULL || output == NULL || output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    output[0] = '\0';
    while (*cursor != '\0') {
        const char *line_end = strchr(cursor, '\n');
        size_t line_length = line_end != NULL ? (size_t)(line_end - cursor) : strlen(cursor);
        if (line_length > 0U && cursor[line_length - 1U] == '\r') {
            --line_length;
        }
        if (line_length > key_length + 1U &&
            memcmp(cursor, key, key_length) == 0 && cursor[key_length] == '=') {
            const size_t value_length = line_length - key_length - 1U;
            ++matches;
            if (strict && matches > 1U) {
                return UMI_STATUS_PARSE_ERROR;
            }
            if (value_length >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            memcpy(output, cursor + key_length + 1U, value_length);
            output[value_length] = '\0';
        }
        if (line_end == NULL) break;
        cursor = line_end + 1;
    }
    return matches == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
}

void umi_ai_helix_parsed_response_init(UmiAiHelixParsedResponse *parsed)
{
    if (parsed != NULL) {
        memset(parsed, 0, sizeof(*parsed));
    }
}

UmiStatus umi_ai_helix_response_parse(const char *text,
                                      int strict_protocol,
                                      UmiAiHelixParsedResponse *parsed)
{
    char disposition[32];
    char action[32];
    char confidence[64];
    char *end = NULL;
    UmiStatus status;
    double value;
    if (text == NULL || parsed == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_ai_helix_parsed_response_init(parsed);
    status = extract_field(text, "disposition", strict_protocol, disposition, sizeof(disposition));
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "action", strict_protocol, action, sizeof(action));
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "target", strict_protocol, parsed->target, sizeof(parsed->target));
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "summary", strict_protocol, parsed->summary, sizeof(parsed->summary));
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "confidence", strict_protocol, confidence, sizeof(confidence));
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "rationale", strict_protocol, parsed->rationale, sizeof(parsed->rationale));
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = umi_ai_helix_protocol_parse_disposition(disposition, &parsed->disposition);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_helix_protocol_parse_action(action, &parsed->action_kind);
    if (status != UMI_STATUS_OK) return status;
    value = strtod(confidence, &end);
    if (end == confidence || end == NULL || *end != '\0' || value < 0.0 || value > 1.0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    parsed->confidence = value;
    return UMI_STATUS_OK;
}
