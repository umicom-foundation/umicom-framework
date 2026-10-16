/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_progress.c
 * PURPOSE: Preserve completed actions when publication rediscovers a changed dependency graph.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "publication_internal.h"
#include <string.h>

UmiStatus umi_publication_remember_progress(UmiPublicationState *state)
{
    if (!state || !state->report || state->report->count > UMI_REPOSITORY_PUBLICATION_CAPACITY ||
        state->progress_count > UMI_REPOSITORY_PUBLICATION_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < state->report->count; ++i) {
        const UmiRepositoryPublicationEntry *entry = &state->report->entries[i];
        if (!entry->committed && !entry->pushed && !entry->fast_forwarded && !entry->locked) continue;
        if (!memchr(entry->path, '\0', sizeof(entry->path))) return UMI_STATUS_INVALID_ARGUMENT;
        size_t record = 0U;
        while (record < state->progress_count &&
            !umi_path_equal(state->progress[record].path, entry->path)) ++record;
        if (record == state->progress_count) {
            /* Refuse more history than can be retained before clearing a report.
             * The existing snapshot remains available on this failure. */
            if (record == UMI_REPOSITORY_PUBLICATION_CAPACITY)
                return umi_publication_fail(state, i, UMI_STATUS_CAPACITY_EXCEEDED,
                    "Too many distinct updated checkouts across graph refreshes; completed work is retained.");
            memset(&state->progress[record], 0, sizeof(state->progress[record]));
            memcpy(state->progress[record].path, entry->path, strlen(entry->path) + 1U);
            ++state->progress_count;
        }
        UmiPublicationProgress *saved = &state->progress[record];
        saved->committed |= entry->committed;
        saved->pushed |= entry->pushed;
        saved->fast_forwarded |= entry->fast_forwarded;
        saved->locked |= entry->locked;
    }
    return UMI_STATUS_OK;
}

void umi_publication_restore_progress(UmiPublicationState *state)
{
    if (!state || !state->report || state->report->count > UMI_REPOSITORY_PUBLICATION_CAPACITY ||
        state->progress_count > UMI_REPOSITORY_PUBLICATION_CAPACITY) return;
    for (size_t i = 0U; i < state->report->count; ++i) {
        UmiRepositoryPublicationEntry *entry = &state->report->entries[i];
        for (size_t j = 0U; j < state->progress_count; ++j) {
            const UmiPublicationProgress *saved = &state->progress[j];
            if (!umi_path_equal(saved->path, entry->path)) continue;
            /* Only completed-action flags survive. Branches, HEADs, topology,
             * dirty state and blockers must always come from fresh inspection. */
            entry->committed |= saved->committed;
            entry->pushed |= saved->pushed;
            entry->fast_forwarded |= saved->fast_forwarded;
            entry->locked |= saved->locked;
            break;
        }
    }
}
