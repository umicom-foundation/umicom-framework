/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/bookmarks.h
 * PURPOSE: Bookmark live source locations through the existing document and navigation owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_BOOKMARKS_H
#define UMICOM_DOCUMENT_BOOKMARKS_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DOCUMENT_BOOKMARK_CAPACITY 64U
typedef struct UmiDocumentBookmark {
    UmiDocumentId document_id;
    char view_id[UMI_UI_ID_CAPACITY];
    char display_name[UMI_DOCUMENT_NAME_CAPACITY];
    uint64_t line, column;
    int open;
} UmiDocumentBookmark;
typedef struct UmiDocumentBookmarksSnapshot {
    size_t count, open_count;
    uint64_t revision;
    int busy;
} UmiDocumentBookmarksSnapshot;
/* Bookmarks belong to this editing session, including untitled and read-only
 * drafts. They retain identity and one-based line/UTF-8 byte column, not text.
 * Save As retains the live identity. Closing and reopening a path creates a
 * different document: an old bookmark never redirects to that new tab.
 * Positions are fixed coordinates, not tracking markers when lines are edited.
 * No bookmark is written to disk. Use the coordinator's owning thread. */
UmiStatus UmiDocumentCoordinatorBookmarks(const UmiDocumentCoordinator *coordinator,
    UmiDocumentBookmarksSnapshot *out_snapshot);
UmiStatus UmiDocumentCoordinatorBookmarkAt(const UmiDocumentCoordinator *coordinator,
    size_t index, UmiDocumentBookmark *out_bookmark);
/* Toggle one bookmark per active document line. Adding captures the current
 * column; toggling anywhere on that same line removes it. out_added is optional
 * and unchanged on failure. Text, Undo, pins and active tab are unchanged. */
UmiStatus UmiDocumentCoordinatorToggleBookmark(UmiDocumentCoordinator *coordinator, int *out_added);
/* Visit next (+1) or previous (-1) in insertion order, wrapping once. If the
 * active line is not bookmarked, start at the first/last row. Closed documents
 * and deleted lines are skipped; no available target returns NOT_FOUND without
 * changing the active tab. Columns clamp as in Go To. Success records a normal
 * navigation jump, permitting Navigate Back. Expected revision comes from the
 * same owner's snapshot. Stale evidence is INVALID_STATE; busy owners are BUSY. */
UmiStatus UmiDocumentCoordinatorTravelBookmark(UmiDocumentCoordinator *coordinator,
    int direction, uint64_t expected_revision);
/* Clear only bookmark metadata, checking the displayed revision first. */
UmiStatus UmiDocumentCoordinatorClearBookmarks(UmiDocumentCoordinator *coordinator,
    uint64_t expected_revision);
#ifdef __cplusplus
}
#endif
#endif
