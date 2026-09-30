/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/snapshot_contracts/main.c
 * PURPOSE: Import bookmark records without publishing a partial failed batch.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/bookmarks.h"
#include <stdio.h>
#include <string.h>

static int Lesson(UmiBookmarkRegistry *bookmarks)
{
    UmiBookmarkSnapshot incoming[2] = {0};
    (void)snprintf(incoming[0].id, sizeof(incoming[0].id), "%s", "notes");
    (void)snprintf(incoming[0].label, sizeof(incoming[0].label), "%s", "Project notes");
    (void)snprintf(incoming[0].uri, sizeof(incoming[0].uri), "%s", "file:///example/notes.txt");
    (void)snprintf(incoming[1].id, sizeof(incoming[1].id), "%s", "manual");
    (void)snprintf(incoming[1].uri, sizeof(incoming[1].uri), "%s", "file:///example/manual.html");

    /* Simulate a producer that filled a field without writing its terminator.
     * The entire batch is rejected, including the valid first bookmark. */
    (void)memset(incoming[1].label, 'x', sizeof(incoming[1].label));
    UmiSnapshotBatchResult result;
    UmiStatus status = umi_platform_bookmarks_registry_upsert_many(bookmarks, incoming, 2U, &result);
    if (status != UMI_STATUS_INVALID_ARGUMENT || result.rejected_index != 1U ||
        result.applied != 0U || umi_platform_bookmarks_registry_count(bookmarks) != 0U) return 1;
    (void)printf("Row %zu, field %s: %s\n", result.rejected_index + 1U,
        result.validation.field, UmiSnapshotIssueText(result.validation.issue));

    (void)snprintf(incoming[1].label, sizeof(incoming[1].label), "%s", "Beginner manual");
    status = umi_platform_bookmarks_registry_upsert_many(bookmarks, incoming, 2U, &result);
    if (status != UMI_STATUS_OK || result.applied != 2U ||
        umi_platform_bookmarks_registry_count(bookmarks) != 2U) return 1;
    UmiBookmarkSnapshot saved;
    status = umi_platform_bookmarks_registry_find(bookmarks, "manual", &saved);
    if (status != UMI_STATUS_OK || strcmp(saved.label, "Beginner manual") != 0) return 1;
    (void)printf("Imported %zu bookmarks. Second label: %s\n", result.applied, saved.label);
    return 0;
}

int main(void)
{
    UmiBookmarkRegistry *bookmarks = NULL;
    UmiStatus status = umi_platform_bookmarks_registry_create(&bookmarks);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot create bookmarks: %s\n", umi_status_text(status));
        return 1;
    }
    int result = Lesson(bookmarks);
    umi_platform_bookmarks_registry_destroy(bookmarks);
    return result;
}
