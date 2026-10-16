/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_progress.c
 * PURPOSE: Verify action evidence survives row reordering without restoring stale Git observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "publication_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    (void)fprintf(stderr, "Progress check failed at line %d: %s\n", __LINE__, #condition); \
    goto finish; } } while (0)

int main(void)
{
    UmiPublicationState *state = calloc(1U, sizeof(*state));
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof(*report));
    int result = EXIT_FAILURE;
    if (!state || !report) goto finish;
    state->report = report;
    report->count = 2U;
    (void)snprintf(report->entries[0].path, sizeof(report->entries[0].path), "/workspace/alpha");
    (void)snprintf(report->entries[1].path, sizeof(report->entries[1].path), "/workspace/beta");
    report->entries[0].fast_forwarded = 1;
    report->entries[0].blocked = 1;
    report->entries[0].changed_paths = 9U;
    report->entries[1].committed = report->entries[1].pushed = report->entries[1].locked = 1;
    CHECK(umi_publication_remember_progress(state) == UMI_STATUS_OK);
    CHECK(state->progress_count == 2U);
    CHECK(umi_publication_remember_progress(state) == UMI_STATUS_OK && state->progress_count == 2U);
    memset(report->entries, 0, sizeof(report->entries));
    report->count = 3U;
    (void)snprintf(report->entries[0].path, sizeof(report->entries[0].path), "/workspace/beta");
    (void)snprintf(report->entries[1].path, sizeof(report->entries[1].path), "/workspace/new");
    (void)snprintf(report->entries[2].path, sizeof(report->entries[2].path), "/workspace/alpha");
    (void)snprintf(report->entries[2].head, sizeof(report->entries[2].head), "fresh observation");
    umi_publication_restore_progress(state);
    CHECK(report->entries[0].committed && report->entries[0].pushed && report->entries[0].locked);
    CHECK(!report->entries[0].fast_forwarded && !report->entries[1].committed &&
        !report->entries[1].fast_forwarded && report->entries[2].fast_forwarded);
    CHECK(!report->entries[2].blocked && report->entries[2].changed_paths == 0U);
    CHECK(strcmp(report->entries[2].head, "fresh observation") == 0);
    CHECK(report->committed_count == 0U && report->pushed_count == 0U && report->updated_count == 0U);
    /* An invocation owns its own history. Clearing it does not manufacture
     * prior actions in a later run or keep index numbers from an older graph. */
    state->progress_count = 0U;
    memset(state->progress, 0, sizeof(state->progress));
    memset(report->entries, 0, sizeof(report->entries));
    umi_publication_restore_progress(state);
    CHECK(!report->entries[0].committed && !report->entries[2].fast_forwarded);
    /* Graph refreshes can expose more distinct completed checkouts than one
     * snapshot can hold. Refuse overflow while retaining the current result
     * and the already remembered paths for the caller's investigation. */
    memset(report, 0, sizeof(*report));
    report->count = UMI_REPOSITORY_PUBLICATION_CAPACITY;
    for (size_t i = 0U; i < report->count; ++i) {
        (void)snprintf(report->entries[i].path, sizeof(report->entries[i].path),
            "/workspace/completed-%zu", i);
        report->entries[i].fast_forwarded = 1;
    }
    CHECK(umi_publication_remember_progress(state) == UMI_STATUS_OK);
    CHECK(state->progress_count == UMI_REPOSITORY_PUBLICATION_CAPACITY);
    report->count = 1U;
    (void)snprintf(report->entries[0].path, sizeof(report->entries[0].path), "/workspace/newly-completed");
    report->updated_count = UMI_REPOSITORY_PUBLICATION_CAPACITY + 1U;
    CHECK(umi_publication_remember_progress(state) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(state->progress_count == UMI_REPOSITORY_PUBLICATION_CAPACITY);
    CHECK(report->count == 1U && report->entries[0].fast_forwarded && report->entries[0].blocked);
    CHECK(report->updated_count == UMI_REPOSITORY_PUBLICATION_CAPACITY + 1U);
    CHECK(strcmp(report->entries[0].path, "/workspace/newly-completed") == 0);
    CHECK(strcmp(state->progress[0].path, "/workspace/completed-0") == 0);
    CHECK(umi_publication_remember_progress(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    result = EXIT_SUCCESS;
finish:
    free(report);
    free(state);
    return result;
}
