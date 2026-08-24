/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/inline_history.c
 *
 * PURPOSE:
 *   Implement bounded inline suggestion history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/inline_history.h"

#include <stdlib.h>
#include <string.h>

struct UmiIdeInlineHistory {
    UmiIdeInlineSuggestion items[UMI_IDE_INLINE_HISTORY_CAPACITY];
    size_t count;
    uint64_t revision;
};

UmiStatus umi_ide_inline_history_create(UmiIdeInlineHistory **out_history)
{
    UmiIdeInlineHistory *history;

    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;

    history = (UmiIdeInlineHistory *)calloc(1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    history->revision = 1U;
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_ide_inline_history_destroy(UmiIdeInlineHistory *history)
{
    free(history);
}

UmiStatus umi_ide_inline_history_record(
    UmiIdeInlineHistory *history,
    const UmiIdeInlineSuggestion *suggestion)
{
    if (history == NULL || suggestion == NULL ||
        suggestion->suggestion_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (history->count == UMI_IDE_INLINE_HISTORY_CAPACITY) {
        (void)memmove(
            &history->items[0],
            &history->items[1],
            (history->count - 1U) * sizeof(history->items[0]));
        history->count -= 1U;
    }

    history->items[history->count++] = *suggestion;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_inline_history_at(
    const UmiIdeInlineHistory *history,
    size_t index,
    UmiIdeInlineSuggestion *out_suggestion)
{
    if (history == NULL || out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= history->count) return UMI_STATUS_NOT_FOUND;

    *out_suggestion = history->items[index];
    return UMI_STATUS_OK;
}

size_t umi_ide_inline_history_count(const UmiIdeInlineHistory *history)
{
    return history != NULL ? history->count : 0U;
}

uint64_t umi_ide_inline_history_revision(const UmiIdeInlineHistory *history)
{
    return history != NULL ? history->revision : 0U;
}
