/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/history.c
 *
 * PURPOSE:
 *   Implement bounded commit-history parsing and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/history.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/vcs/commit.h"

struct UmiVcsHistory {
    UmiVcsCommit *items;
    size_t count;
};

UmiStatus umi_vcs_history_create(UmiVcsHistory **out_history)
{
    UmiVcsHistory *history;
    if (out_history == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_history = NULL;
    history = calloc(1U, sizeof(*history));
    if (history == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    history->items = calloc(UMI_VCS_MAX_COMMITS,
                            sizeof(history->items[0]));
    if (history->items == NULL) {
        free(history);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_vcs_history_destroy(UmiVcsHistory *history)
{
    if (history != NULL) {
        free(history->items);
        free(history);
    }
}

void umi_vcs_history_clear(UmiVcsHistory *history)
{
    if (history != NULL) {
        history->count = 0U;
    }
}

UmiStatus umi_vcs_history_parse(UmiVcsHistory *history,
                                const char *output,
                                size_t *out_parsed)
{
    const char *cursor;
    const char *end;
    char record[4096];
    size_t length;
    size_t parsed = 0U;
    if (history == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_vcs_history_clear(history);
    cursor = output;
    while (*cursor != '\0' && history->count < UMI_VCS_MAX_COMMITS) {
        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        if (length >= sizeof(record)) {
            length = sizeof(record) - 1U;
        }
        (void)memcpy(record, cursor, length);
        record[length] = '\0';
        if (length > 0U &&
            umi_vcs_commit_parse_record(record,
                                        &history->items[history->count]) ==
                UMI_STATUS_OK) {
            history->count += 1U;
            parsed += 1U;
        }
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }
    if (out_parsed != NULL) {
        *out_parsed = parsed;
    }
    return UMI_STATUS_OK;
}

size_t umi_vcs_history_count(const UmiVcsHistory *history)
{
    return history != NULL ? history->count : 0U;
}

const UmiVcsCommit *umi_vcs_history_at(const UmiVcsHistory *history,
                                       size_t index)
{
    return history != NULL && index < history->count
        ? &history->items[index]
        : NULL;
}
