/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/navigation_history.c
 *
 * PURPOSE:
 *   Implement bounded back/forward cross-domain navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/navigation_history.h"

#include <stdlib.h>
#include <string.h>

struct UmiIdeNavigationHistory {
    UmiIdeNavigationTarget items[UMI_IDE_INTEGRATION_NAVIGATION_HISTORY_CAPACITY];
    size_t count;
    size_t cursor;
    uint64_t sequence;
    uint64_t revision;
};

/*
 * Initialise ide navigation history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ide_navigation_history_create(
    UmiIdeNavigationHistory **out_history)
{
    UmiIdeNavigationHistory *history;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;

    history = (UmiIdeNavigationHistory *)calloc(1U, sizeof(*history));
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
 * Release or reset state held by ide navigation history so the same storage can be reused
 * safely.
 */
void umi_ide_navigation_history_destroy(UmiIdeNavigationHistory *history)
{
    free(history);
}

/*
 * Provide the ide navigation history visit operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_navigation_history_visit(
    UmiIdeNavigationHistory *history,
    const UmiIdeNavigationTarget *target)
{
    UmiIdeNavigationTarget copy;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || target == NULL ||
        !umi_ide_location_has_target(&target->location)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    copy = *target;
    copy.sequence = ++history->sequence;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count > 0U &&
        history->cursor + 1U < history->count) {
        history->count = history->cursor + 1U;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count ==
        UMI_IDE_INTEGRATION_NAVIGATION_HISTORY_CAPACITY) {
        (void)memmove(
            &history->items[0],
            &history->items[1],
            (history->count - 1U) * sizeof(history->items[0]));
        history->count -= 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (history->cursor > 0U) history->cursor -= 1U;
    }

    history->items[history->count++] = copy;
    history->cursor = history->count - 1U;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ide navigation history current operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_navigation_history_current(
    const UmiIdeNavigationHistory *history,
    UmiIdeNavigationTarget *out_target)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_target = history->items[history->cursor];
    return UMI_STATUS_OK;
}

/*
 * Provide the ide navigation history back operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_navigation_history_back(
    UmiIdeNavigationHistory *history,
    UmiIdeNavigationTarget *out_target)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count == 0U || history->cursor == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    history->cursor -= 1U;
    history->revision += 1U;
    *out_target = history->items[history->cursor];
    return UMI_STATUS_OK;
}

/*
 * Provide the ide navigation history forward operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_navigation_history_forward(
    UmiIdeNavigationHistory *history,
    UmiIdeNavigationTarget *out_target)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count == 0U ||
        history->cursor + 1U >= history->count) {
        return UMI_STATUS_NOT_FOUND;
    }

    history->cursor += 1U;
    history->revision += 1U;
    *out_target = history->items[history->cursor];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ide navigation history without changing
 * their state.
 */
size_t umi_ide_navigation_history_count(
    const UmiIdeNavigationHistory *history)
{
    return history != NULL ? history->count : 0U;
}

/*
 * Provide the ide navigation history revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ide_navigation_history_revision(
    const UmiIdeNavigationHistory *history)
{
    return history != NULL ? history->revision : 0U;
}
