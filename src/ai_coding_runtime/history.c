/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/history.c
 *
 * PURPOSE:
 *   Implement bounded oldest-first agent snapshot retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/history.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingHistory {
    UmiAiCodingAgentSnapshot
        items[UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY];
    size_t count;
};

/*
 * Initialise ai coding history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_history_create(UmiAiCodingHistory **out_history)
{
    UmiAiCodingHistory *history;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;

    history = (UmiAiCodingHistory *)calloc(1U, sizeof(*history));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_history = history;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding history so the same storage can be reused
 * safely.
 */
void umi_ai_coding_history_destroy(UmiAiCodingHistory *history)
{
    free(history);
}

/*
 * Provide the ai coding history record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_history_record(
    UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count == UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY) {
        (void)memmove(
            &history->items[0],
            &history->items[1],
            (history->count - 1U) * sizeof(history->items[0]));
        history->count -= 1U;
    }

    history->items[history->count++] = *snapshot;
    return UMI_STATUS_OK;
}

/*
 * Find ai coding history while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_coding_history_at(
    const UmiAiCodingHistory *history,
    size_t index,
    UmiAiCodingAgentSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= history->count) return UMI_STATUS_NOT_FOUND;

    *out_snapshot = history->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai coding history without changing their
 * state.
 */
size_t umi_ai_coding_history_count(const UmiAiCodingHistory *history)
{
    return history != NULL ? history->count : 0U;
}
