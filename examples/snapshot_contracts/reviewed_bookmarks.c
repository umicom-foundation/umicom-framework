/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/snapshot_contracts/reviewed_bookmarks.c
 * PURPOSE: Review an independent capture and refuse a stale publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/bookmarks.h"
#include <stdio.h>
#include <string.h>

/* Prepare an edit using an independent capture. A real editor can display
 * these rows while a background refresh prepares a competing proposal. */
static int ReviewBookmarks(UmiBookmarkRegistry *bookmarks)
{
    UmiBookmarkSnapshot original = {0};
    (void)snprintf(original.id, sizeof(original.id), "%s", "notes");
    (void)snprintf(original.label, sizeof(original.label), "%s", "Project notes");
    if (umi_platform_bookmarks_registry_upsert(bookmarks, &original) != UMI_STATUS_OK) return 1;

    UmiBookmarkSnapshot proposed[1];
    UmiSnapshotCapture capture;
    if (umi_platform_bookmarks_registry_capture(bookmarks, proposed, 1U, &capture) != UMI_STATUS_OK) return 1;
    (void)snprintf(proposed[0].label, sizeof(proposed[0].label), "%s", "Reviewed notes");

    /* Another owner-thread action changes the collection after our capture.
     * Publishing the old proposal must refuse instead of silently losing it. */
    (void)snprintf(original.label, sizeof(original.label), "%s", "More recent notes");
    if (umi_platform_bookmarks_registry_upsert(bookmarks, &original) != UMI_STATUS_OK) return 1;
    UmiStatus status = umi_platform_bookmarks_registry_replace_if_current(
        bookmarks, capture.revision, proposed, 1U, NULL);
    if (status != UMI_STATUS_INVALID_STATE) return 1;
    puts("The earlier proposal was refused; the newer notes remain available.");

    /* Recapture and review again before making a new decision. This lesson
     * deliberately chooses the reviewed label; an application should ask its
     * user to resolve the conflict rather than retrying automatically. */
    if (umi_platform_bookmarks_registry_capture(bookmarks, proposed, 1U, &capture) != UMI_STATUS_OK) return 1;
    if (strcmp(proposed[0].label, "More recent notes") != 0) return 1;
    (void)snprintf(proposed[0].label, sizeof(proposed[0].label), "%s", "Reviewed notes");
    if (umi_platform_bookmarks_registry_replace_if_current(
            bookmarks, capture.revision, proposed, 1U, NULL) != UMI_STATUS_OK) return 1;
    UmiBookmarkSnapshot published;
    if (umi_platform_bookmarks_registry_find(bookmarks, "notes", &published) != UMI_STATUS_OK) return 1;
    if (strcmp(published.label, "Reviewed notes") != 0) return 1;
    puts("The newly reviewed proposal was published as one complete collection.");

    /* Clearing is a reviewed publication too. Reusing the pre-replacement
     * capture must not erase a collection that changed after that review. */
    if (umi_platform_bookmarks_registry_clear_if_current(bookmarks, capture.revision) !=
            UMI_STATUS_INVALID_STATE) return 1;
    if (umi_platform_bookmarks_registry_count(bookmarks) != 1U) return 1;
    if (umi_platform_bookmarks_registry_capture(bookmarks, proposed, 1U, &capture) !=
            UMI_STATUS_OK) return 1;
    if (strcmp(proposed[0].label, "Reviewed notes") != 0) return 1;
    if (umi_platform_bookmarks_registry_clear_if_current(bookmarks, capture.revision) !=
            UMI_STATUS_OK) return 1;
    if (umi_platform_bookmarks_registry_count(bookmarks) != 0U ||
        umi_platform_bookmarks_registry_revision(bookmarks) != capture.revision + 1U) return 1;
    uint64_t empty_revision = umi_platform_bookmarks_registry_revision(bookmarks);
    if (umi_platform_bookmarks_registry_clear_if_current(bookmarks, empty_revision) !=
            UMI_STATUS_OK || umi_platform_bookmarks_registry_revision(bookmarks) != empty_revision) return 1;
    /* Captured rows belong to the caller even after the registry is cleared. */
    if (strcmp(proposed[0].label, "Reviewed notes") != 0) return 1;
    puts("A stale clear was refused; a current clear removed the reviewed collection once.");

    return 0;
}

int main(void)
{
    UmiBookmarkRegistry *bookmarks = NULL;
    if (umi_platform_bookmarks_registry_create(&bookmarks) != UMI_STATUS_OK) return 1;
    int result = ReviewBookmarks(bookmarks);
    umi_platform_bookmarks_registry_destroy(bookmarks);
    return result;
}
