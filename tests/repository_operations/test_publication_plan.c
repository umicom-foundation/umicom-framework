/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_plan.c
 *
 * PURPOSE:
 *   Verify duplicate ownership, dependency ordering and fail-closed graph planning.
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
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    (void)fprintf(stderr, "Check failed at line %d: %s\n", __LINE__, #condition); \
    free(state); free(report); return EXIT_FAILURE; } } while (0)

static UmiStatus lock_preview(void *context, const char *path, int dry_run)
{
    size_t *calls = context;
    (void)path;
    if (!dry_run) return UMI_STATUS_PERMISSION_DENIED;
    ++*calls;
    return UMI_STATUS_OK;
}

static void fill(UmiPublicationState *state, UmiRepositoryPublicationReport *report)
{
    memset(state, 0, sizeof(*state));
    memset(report, 0, sizeof(*report));
    state->report = report;
    report->count = 4U;
    /* Parent has a duplicate Framework copy and a separate application.
     * A source edit in Framework's primary copy must publish first. */
    (void)snprintf(state->identity[0], 1024U, "host/framework");
    (void)snprintf(state->identity[1], 1024U, "host/suite");
    (void)snprintf(state->identity[2], 1024U, "host/framework");
    (void)snprintf(state->identity[3], 1024U, "host/app");
    for (size_t i = 0U; i < report->count; ++i) {
        report->entries[i].parent = SIZE_MAX;
        report->entries[i].primary = i;
        memset(report->entries[i].head, 'a', 40U);
        memset(report->entries[i].remote_head, 'a', 40U);
    }
    report->entries[0].changed_paths = 3U;
    state->edges[0].parent = 1U; state->edges[0].child = 2U;
    state->edges[1].parent = 1U; state->edges[1].child = 3U;
    state->edge_count = 2U;
}

int main(void)
{
    UmiPublicationState *state = calloc(1U, sizeof(*state));
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof(*report));
    size_t calls = 0U;
    CHECK(state && report);
    fill(state, report);
    state->backend.context = &calls;
    state->backend.lock = lock_preview;
    CHECK(umi_publication_plan(state) == UMI_STATUS_OK);
    CHECK(report->repository_count == 3U && state->order_count == 3U);
    CHECK(report->entries[2].primary == 0U);
    CHECK(state->order[0] == 0U && state->order[1] == 3U && state->order[2] == 1U);
    CHECK(calls == 1U);

    fill(state, report);
    state->backend.context = &calls; state->backend.lock = lock_preview;
    report->entries[2].changed_paths = 1U;
    CHECK(umi_publication_plan(state) == UMI_STATUS_INVALID_STATE);
    CHECK(report->blocked_count == 1U);

    fill(state, report);
    state->backend.context = &calls; state->backend.lock = lock_preview;
    report->entries[0].changed_paths = 0U;
    report->entries[0].ahead = report->entries[2].ahead = 1U;
    report->entries[2].head[0] = 'b';
    CHECK(umi_publication_plan(state) == UMI_STATUS_INVALID_STATE);

    fill(state, report);
    state->backend.context = &calls; state->backend.lock = lock_preview;
    state->edges[2].parent = 0U; state->edges[2].child = 1U;
    state->edge_count = 3U;
    CHECK(umi_publication_plan(state) == UMI_STATUS_INVALID_STATE);

    fill(state, report);
    state->backend.context = &calls; state->backend.lock = lock_preview;
    report->entries[2].remote_head[0] = 'b';
    CHECK(umi_publication_plan(state) == UMI_STATUS_INVALID_STATE);

    fill(state, report);
    state->backend.context = &calls; state->backend.lock = lock_preview;
    report->entries[3].vendor = 1;
    CHECK(umi_publication_plan(state) == UMI_STATUS_OK);
    CHECK(state->order_count == 2U);
    free(state);
    free(report);
    return EXIT_SUCCESS;
}
