/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/inline_history.c
 *
 * PURPOSE:
 *   Implement bounded inline suggestion history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/inline_history.h"

#include <stdlib.h>
#include <string.h>

struct UmiIdeInlineHistory {
    UmiIdeInlineSuggestion items[UMI_IDE_INLINE_HISTORY_CAPACITY];
    size_t count;
    uint64_t revision;
};

/*
 * Initialise ide inline history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_inline_history_create(UmiIdeInlineHistory **out_history)
{
    UmiIdeInlineHistory *history;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;

    history = (UmiIdeInlineHistory *)calloc(1U, sizeof(*history));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    history->revision = 1U;
    *out_history = history;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ide inline history so the same storage can be reused
 * safely.
 */
void umi_ide_inline_history_destroy(UmiIdeInlineHistory *history)
{
    free(history);
}

/*
 * Provide the ide inline history record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_history_record(
    UmiIdeInlineHistory *history,
    const UmiIdeInlineSuggestion *suggestion)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || suggestion == NULL ||
        suggestion->suggestion_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Find ide inline history while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ide_inline_history_at(
    const UmiIdeInlineHistory *history,
    size_t index,
    UmiIdeInlineSuggestion *out_suggestion)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= history->count) return UMI_STATUS_NOT_FOUND;

    *out_suggestion = history->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ide inline history without changing their
 * state.
 */
size_t umi_ide_inline_history_count(const UmiIdeInlineHistory *history)
{
    return history != NULL ? history->count : 0U;
}

/*
 * Provide the ide inline history revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ide_inline_history_revision(const UmiIdeInlineHistory *history)
{
    return history != NULL ? history->revision : 0U;
}
