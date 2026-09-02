/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/history.c
 *
 * PURPOSE:
 *   Implement bounded commit-history parsing and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/history.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/vcs/commit.h"

struct UmiVcsHistory {
    UmiVcsCommit *items;
    size_t count;
    size_t capacity;
};
/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiVcsHistory *history)
{
    UmiVcsCommit *resized; size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count < history->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->capacity >= UMI_VCS_MAX_COMMITS) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = history->capacity == 0U ? 64U : history->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_VCS_MAX_COMMITS) next = UMI_VCS_MAX_COMMITS;
    resized = realloc(history->items, next * sizeof(history->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->items = resized; history->capacity = next; return UMI_STATUS_OK;
}

/*
 * Initialise vcs history from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_vcs_history_create(UmiVcsHistory **out_history)
{
    UmiVcsHistory *history;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_history == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_history = NULL;
    history = calloc(1U, sizeof(*history));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    history->capacity = 64U;
    history->items = calloc(history->capacity,
                            sizeof(history->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history->items == NULL) {
        free(history);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_history = history;
    return UMI_STATUS_OK;
}

/* Release or reset state held by vcs history so the same storage can be reused safely. */
void umi_vcs_history_destroy(UmiVcsHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history != NULL) {
        free(history->items);
        free(history);
    }
}

/* Release or reset state held by vcs history so the same storage can be reused safely. */
void umi_vcs_history_clear(UmiVcsHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history != NULL) {
        history->count = 0U;
    }
}

/*
 * Read vcs history into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_vcs_history_parse(UmiVcsHistory *history,
                                const char *output,
                                size_t *out_parsed)
{
    const char *cursor;
    const char *end;
    char record[4096];
    size_t length;
    size_t parsed = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_vcs_history_clear(history);
    cursor = output;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0' && history->count < UMI_VCS_MAX_COMMITS) {
        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(record)) {
            length = sizeof(record) - 1U;
        }
        (void)memcpy(record, cursor, length);
        record[length] = '\0';
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length > 0U) {
            UmiStatus capacity_status = ensure_capacity(history);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (capacity_status != UMI_STATUS_OK) return capacity_status;
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_vcs_commit_parse_record(
                    record,
                    &history->items[history->count]) == UMI_STATUS_OK) {
                history->count += 1U;
                parsed += 1U;
            }
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_parsed != NULL) {
        *out_parsed = parsed;
    }
    return UMI_STATUS_OK;
}

/* Return the number of records represented by vcs history without changing their state. */
size_t umi_vcs_history_count(const UmiVcsHistory *history)
{
    return history != NULL ? history->count : 0U;
}

/* Find vcs history while leaving the underlying catalogue or model owned by this module. */
const UmiVcsCommit *umi_vcs_history_at(const UmiVcsHistory *history,
                                       size_t index)
{
    return history != NULL && index < history->count
        ? &history->items[index]
        : NULL;
}
