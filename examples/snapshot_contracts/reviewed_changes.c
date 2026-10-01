/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/snapshot_contracts/reviewed_changes.c
 * PURPOSE: Review a mixed bookmark change and observe rollback after a missing removal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/bookmarks.h"
#include <stdio.h>
#include <string.h>

/* IDs belong to this small example. Real applications retain their existing
 * identity policy and populate any other fields required by their domain. */
static UmiBookmarkSnapshot Bookmark(const char *id, const char *label)
{
    UmiBookmarkSnapshot item = {0};
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.label, sizeof(item.label), "%s", label);
    return item;
}

static int ReviewChanges(UmiBookmarkRegistry *bookmarks)
{
    UmiBookmarkSnapshot original = Bookmark("draft", "Draft notes");
    UmiBookmarkSnapshot neighbour = Bookmark("reference", "Reference material");
    if (umi_platform_bookmarks_registry_upsert(bookmarks, &original) != UMI_STATUS_OK) return 1;
    if (umi_platform_bookmarks_registry_upsert(bookmarks, &neighbour) != UMI_STATUS_OK) return 1;
    UmiBookmarkSnapshot review[2];
    UmiSnapshotCapture capture;
    if (umi_platform_bookmarks_registry_capture(bookmarks, review, 2U, &capture) != UMI_STATUS_OK) return 1;

    /* This proposed insertion cannot be published if its paired removal fails.
     * A UI would keep the proposal visible and explain which edit was refused. */
    UmiBookmarkEdit changes[2] = {
        {UMI_SNAPSHOT_EDIT_UPSERT, Bookmark("notes", "Reviewed notes")},
        {UMI_SNAPSHOT_EDIT_REMOVE, Bookmark("missing", "")}
    };
    UmiSnapshotBatchResult result;
    UmiStatus status = umi_platform_bookmarks_registry_edit_if_current(
        bookmarks, capture.revision, changes, 2U, &result);
    if (status != UMI_STATUS_NOT_FOUND || result.rejected_index != 1U || result.applied != 0U) return 1;
    UmiBookmarkSnapshot row;
    if (umi_platform_bookmarks_registry_find(bookmarks, "notes", &row) != UMI_STATUS_NOT_FOUND) return 1;
    if (umi_platform_bookmarks_registry_find(bookmarks, "draft", &row) != UMI_STATUS_OK) return 1;
    puts("Missing removal: the draft remains and no partial insertion was published.");

    /* The example now makes a deliberate new decision: remove the captured
     * draft, then add the reviewed notes. The unrelated bookmark is retained. */
    changes[0].kind = UMI_SNAPSHOT_EDIT_REMOVE;
    changes[0].item = review[0];
    changes[1].kind = UMI_SNAPSHOT_EDIT_UPSERT;
    changes[1].item = Bookmark("notes", "Reviewed notes");
    status = umi_platform_bookmarks_registry_edit_if_current(
        bookmarks, capture.revision, changes, 2U, &result);
    if (status != UMI_STATUS_OK || result.applied != 2U) return 1;
    if (umi_platform_bookmarks_registry_find(bookmarks, "draft", &row) != UMI_STATUS_NOT_FOUND) return 1;
    if (umi_platform_bookmarks_registry_find(bookmarks, "reference", &row) != UMI_STATUS_OK) return 1;
    if (strcmp(row.label, "Reference material") != 0) return 1;
    if (umi_platform_bookmarks_registry_find(bookmarks, "notes", &row) != UMI_STATUS_OK) return 1;
    if (strcmp(row.label, "Reviewed notes") != 0) return 1;
    puts("Reviewed edits published together; the unrelated reference remains.");

    /* Reusing that old capture is a conflict, even if the same intent is sent
     * again. A caller must obtain another review rather than force a retry. */
    status = umi_platform_bookmarks_registry_edit_if_current(
        bookmarks, capture.revision, changes, 2U, NULL);
    if (status != UMI_STATUS_INVALID_STATE) return 1;
    puts("Repeating the old review was refused as stale.");
    return 0;
}

int main(void)
{
    UmiBookmarkRegistry *bookmarks = NULL;
    if (umi_platform_bookmarks_registry_create(&bookmarks) != UMI_STATUS_OK) return 1;
    int result = ReviewChanges(bookmarks);
    umi_platform_bookmarks_registry_destroy(bookmarks);
    return result;
}
