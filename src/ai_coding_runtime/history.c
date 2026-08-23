/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/history.c
 *
 * PURPOSE:
 *   Implement bounded oldest-first agent snapshot retention.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/history.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingHistory {
    UmiAiCodingAgentSnapshot
        items[UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY];
    size_t count;
};

UmiStatus umi_ai_coding_history_create(UmiAiCodingHistory **out_history)
{
    UmiAiCodingHistory *history;

    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;

    history = (UmiAiCodingHistory *)calloc(1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_ai_coding_history_destroy(UmiAiCodingHistory *history)
{
    free(history);
}

UmiStatus umi_ai_coding_history_record(
    UmiAiCodingHistory *history,
    const UmiAiCodingAgentSnapshot *snapshot)
{
    if (history == NULL || snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

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

UmiStatus umi_ai_coding_history_at(
    const UmiAiCodingHistory *history,
    size_t index,
    UmiAiCodingAgentSnapshot *out_snapshot)
{
    if (history == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= history->count) return UMI_STATUS_NOT_FOUND;

    *out_snapshot = history->items[index];
    return UMI_STATUS_OK;
}

size_t umi_ai_coding_history_count(const UmiAiCodingHistory *history)
{
    return history != NULL ? history->count : 0U;
}
