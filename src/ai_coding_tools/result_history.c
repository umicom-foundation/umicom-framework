/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/result_history.c
 *
 * PURPOSE:
 *   Implement bounded oldest-first coding-tool result retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/result_history.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingToolResultHistory {
    UmiAiCodingToolResult
        items[UMI_AI_CODING_TOOL_RESULT_HISTORY_CAPACITY];
    size_t count;
};

/*
 * Initialise ai coding tool result history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_tool_result_history_create(
    UmiAiCodingToolResultHistory **out_history)
{
    UmiAiCodingToolResultHistory *history;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;

    history = (UmiAiCodingToolResultHistory *)calloc(1U, sizeof(*history));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_history = history;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding tool result history so the same storage can be
 * reused safely.
 */
void umi_ai_coding_tool_result_history_destroy(
    UmiAiCodingToolResultHistory *history)
{
    free(history);
}

/*
 * Provide the ai coding tool result history record operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_result_history_record(
    UmiAiCodingToolResultHistory *history,
    const UmiAiCodingToolResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count == UMI_AI_CODING_TOOL_RESULT_HISTORY_CAPACITY) {
        (void)memmove(
            &history->items[0],
            &history->items[1],
            (history->count - 1U) * sizeof(history->items[0]));
        history->count -= 1U;
    }

    history->items[history->count++] = *result;
    return UMI_STATUS_OK;
}

/*
 * Find ai coding tool result history while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ai_coding_tool_result_history_at(
    const UmiAiCodingToolResultHistory *history,
    size_t index,
    UmiAiCodingToolResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= history->count) return UMI_STATUS_NOT_FOUND;

    *out_result = history->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai coding tool result history without
 * changing their state.
 */
size_t umi_ai_coding_tool_result_history_count(
    const UmiAiCodingToolResultHistory *history)
{
    return history != NULL ? history->count : 0U;
}
