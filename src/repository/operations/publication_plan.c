/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_plan.c
 *
 * PURPOSE:
 *   Select one publishing copy per remote and order the repository dependency graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include <stdio.h>
#include <string.h>

static UmiStatus visit_group(UmiPublicationState *state, size_t primary)
{
    if (state->visiting[primary] == 2U) return UMI_STATUS_OK;
    if (state->visiting[primary] == 1U)
        return umi_publication_fail(state, primary, UMI_STATUS_INVALID_STATE,
            "Repository dependency cycle prevents child-before-parent publication.");
    state->visiting[primary] = 1U;
    /* Every duplicate copy contributes dependencies. A standalone application's
     * nested module may share an origin with a module under another suite. */
    for (size_t e = 0U; e < state->edge_count; ++e) {
        UmiPublicationEdge *edge = &state->edges[e];
        if (state->report->entries[edge->parent].primary == primary &&
            !state->report->entries[edge->child].vendor) {
            UmiStatus status = visit_group(state,
                state->report->entries[edge->child].primary);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    state->visiting[primary] = 2U;
    state->order[state->order_count++] = primary;
    return UMI_STATUS_OK;
}

UmiStatus umi_publication_plan(UmiPublicationState *state)
{
    UmiRepositoryPublicationReport *report = state->report;
    unsigned char assigned[UMI_REPOSITORY_PUBLICATION_CAPACITY] = {0};
    report->repository_count = 0U;
    state->order_count = 0U;
    memset(state->visiting, 0, sizeof(state->visiting));
    for (size_t i = 0U; i < report->count; ++i) {
        size_t primary = i;
        if (assigned[i] || report->entries[i].vendor) continue;
        for (size_t j = i + 1U; j < report->count; ++j) {
            UmiRepositoryPublicationEntry *candidate = &report->entries[j];
            UmiRepositoryPublicationEntry *selected = &report->entries[primary];
            if (candidate->vendor || strcmp(state->identity[i], state->identity[j]) != 0)
                continue;
            if (strcmp(candidate->remote_head, selected->remote_head) != 0)
                return umi_publication_fail(state, j, UMI_STATUS_INVALID_STATE,
                    "Copies observed different origin/main revisions; retry the preview.");
            if ((candidate->changed_paths && selected->changed_paths) ||
                ((candidate->ahead || candidate->changed_paths) &&
                 (selected->ahead || selected->changed_paths) &&
                 strcmp(candidate->head, selected->head) != 0))
                return umi_publication_fail(state, j, UMI_STATUS_INVALID_STATE,
                    "More than one copy has independent work for this remote; preserve and reconcile it first.");
            if (candidate->changed_paths ||
                (candidate->ahead && !selected->changed_paths) ||
                (!selected->ahead && !selected->changed_paths &&
                 candidate->behind < selected->behind))
                primary = j;
        }
        ++report->repository_count;
        for (size_t j = i; j < report->count; ++j) {
            if (!report->entries[j].vendor &&
                strcmp(state->identity[i], state->identity[j]) == 0) {
                report->entries[j].primary = primary;
                assigned[j] = 1U;
            }
        }
    }
    for (size_t i = 0U; i < report->count; ++i) {
        if (report->entries[i].vendor) continue;
        int parent = 0;
        for (size_t e = 0U; e < state->edge_count; ++e)
            if (state->edges[e].parent == i) parent = 1;
        /* Reuse the existing lock validator before staging any source. */
        if (parent) {
            UmiStatus status = state->backend.lock(
                state->backend.context, report->entries[i].path, 1);
            if (status != UMI_STATUS_OK) return umi_publication_fail(state, i, status,
                "Parent submodule-lock validation failed; no publication started.");
        }
        if (report->entries[i].primary == i) {
            UmiStatus status = visit_group(state, i);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}
