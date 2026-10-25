/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/bookmark_selection.h
 *
 * PURPOSE:
 *   Select and remove displayed bookmarks through their authoritative document owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_BOOKMARK_SELECTION_H
#define UMICOM_DOCUMENT_BOOKMARK_SELECTION_H
#include "umicom/document/bookmarks.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Select one displayed bookmark by its index and observed bookmark revision.
 * Closed documents and removed lines return NOT_FOUND without moving the tab.
 * Save As retains identity, but reopening a closed path does not. A successful
 * visit uses normal navigation history and preserves draft text and Undo.
 * Coordinates remain fixed, with columns clamped by the current draft resolver. */
UmiStatus UmiDocumentCoordinatorVisitBookmark(UmiDocumentCoordinator *coordinator, size_t index,
                                              uint64_t expected_revision);
/* Remove one displayed marker, including a closed marker. Stale revision,
 * out-of-range index, busy owner and revision exhaustion preserve all markers.
 * Removal never closes a document or alters text, cursor, file or history.
 * Both calls use the coordinator owner thread and its existing lifetime rules. */
UmiStatus UmiDocumentCoordinatorRemoveBookmark(UmiDocumentCoordinator *coordinator, size_t index,
                                               uint64_t expected_revision);
#ifdef __cplusplus
}
#endif
#endif
