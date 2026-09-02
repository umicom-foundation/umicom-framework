/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/helix/response_parser.c
 *
 * PURPOSE:
 *   Parse six bounded response fields and reject malformed or ambiguous model output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/helix/response_parser.h"
#include "umicom/ai/helix/protocol.h"
#include <stdlib.h>
#include <string.h>

/* Provide the extract field operation used by this module and its client applications. */
static UmiStatus extract_field(const char *text,
                               const char *key,
                               int strict,
                               char *output,
                               size_t output_capacity)
{
    const size_t key_length = strlen(key);
    const char *cursor = text;
    size_t matches = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || key == NULL || output == NULL || output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    output[0] = '\0';
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *line_end = strchr(cursor, '\n');
        size_t line_length = line_end != NULL ? (size_t)(line_end - cursor) : strlen(cursor);
        /* Apply this branch only when its contract condition is satisfied. */
        if (line_length > 0U && cursor[line_length - 1U] == '\r') {
            --line_length;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (line_length > key_length + 1U &&
            memcmp(cursor, key, key_length) == 0 && cursor[key_length] == '=') {
            const size_t value_length = line_length - key_length - 1U;
            ++matches;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strict && matches > 1U) {
                return UMI_STATUS_PARSE_ERROR;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (value_length >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            memcpy(output, cursor + key_length + 1U, value_length);
            output[value_length] = '\0';
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (line_end == NULL) break;
        cursor = line_end + 1;
    }
    return matches == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
}

/*
 * Initialise ai helix parsed response from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_helix_parsed_response_init(UmiAiHelixParsedResponse *parsed)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (parsed != NULL) {
        memset(parsed, 0, sizeof(*parsed));
    }
}

/*
 * Read ai helix response into validated module state and return a status when input cannot
 * be used.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || parsed == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_ai_helix_parsed_response_init(parsed);
    status = extract_field(text, "disposition", strict_protocol, disposition, sizeof(disposition));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "action", strict_protocol, action, sizeof(action));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "target", strict_protocol, parsed->target, sizeof(parsed->target));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "summary", strict_protocol, parsed->summary, sizeof(parsed->summary));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "confidence", strict_protocol, confidence, sizeof(confidence));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = extract_field(text, "rationale", strict_protocol, parsed->rationale, sizeof(parsed->rationale));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    status = umi_ai_helix_protocol_parse_disposition(disposition, &parsed->disposition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_helix_protocol_parse_action(action, &parsed->action_kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value = strtod(confidence, &end);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (end == confidence || end == NULL || *end != '\0' || value < 0.0 || value > 1.0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    parsed->confidence = value;
    return UMI_STATUS_OK;
}
