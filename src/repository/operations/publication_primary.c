/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_primary.c
 *
 * PURPOSE:
 *   Keep duplicate repository publication in an explicitly selected editing directory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include <string.h>

UmiStatus umi_publication_primary_validate(UmiPublicationState *state)
{
    if (!state->primary_root[0]) return UMI_STATUS_OK;
    /* A misspelled directory must not silently fall back to discovery order.
     * Discovery already verifies each checkout and rejects linked ancestors. */
    for (size_t i = 0U; i < state->report->count; ++i)
        if (!state->report->entries[i].vendor &&
            umi_path_is_within(state->primary_root, state->report->entries[i].path))
            return UMI_STATUS_OK;
    return umi_publication_fail(state, SIZE_MAX, UMI_STATUS_INVALID_ARGUMENT,
        "The primary directory contains no discovered publishing checkout.");
}

UmiStatus umi_publication_primary_select(UmiPublicationState *state,
    size_t group, size_t *primary)
{
    size_t preferred = SIZE_MAX;
    if (!state->primary_root[0]) return UMI_STATUS_OK;
    for (size_t i = 0U; i < state->report->count; ++i) {
        UmiRepositoryPublicationEntry *entry = &state->report->entries[i];
        if (entry->vendor || strcmp(state->identity[group], state->identity[i]) != 0 ||
            !umi_path_is_within(state->primary_root, entry->path)) continue;
        if (preferred != SIZE_MAX)
            return umi_publication_fail(state, i, UMI_STATUS_INVALID_STATE,
                "Multiple copies of one remote are inside the primary directory; choose a narrower directory.");
        preferred = i;
    }
    if (preferred == SIZE_MAX) return UMI_STATUS_OK;
    /* Do not promote an edited dependency into the primary role, or discard its
     * commits by preferring a clean copy. Identical unpublished HEADs may be
     * published once; a different unpublished history requires reconciliation. */
    for (size_t i = 0U; i < state->report->count; ++i) {
        UmiRepositoryPublicationEntry *entry = &state->report->entries[i];
        if (i == preferred || entry->vendor ||
            strcmp(state->identity[group], state->identity[i]) != 0) continue;
        if (entry->changed_paths || (entry->ahead &&
                strcmp(entry->head, state->report->entries[preferred].head) != 0))
            return umi_publication_fail(state, i, UMI_STATUS_INVALID_STATE,
                "A copy outside the primary directory has independent work; preserve and reconcile it before publication.");
    }
    *primary = preferred;
    return UMI_STATUS_OK;
}
