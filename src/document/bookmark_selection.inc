/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/bookmark_selection.inc
 *
 * PURPOSE:
 *   Resolve explicit bookmark rows without retargeting closed documents or discarding drafts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/bookmark_selection.h"

/* Direct row navigation uses the same checked draft resolver as Next/Previous.
 * Copy the destination before navigation can notify model observers. */
UmiStatus UmiDocumentCoordinatorVisitBookmark(UmiDocumentCoordinator *coordinator, size_t index,
                                              uint64_t expected_revision) {
  UmiStatus status = BookmarkReady(coordinator, expected_revision);
  if (status != UMI_STATUS_OK)
    return status;
  if (index >= coordinator->bookmark_count)
    return UMI_STATUS_NOT_FOUND;
  UmiDocumentBookmark bookmark = coordinator->bookmarks[index];
  if (!BookmarkOpen(coordinator, &bookmark))
    return UMI_STATUS_NOT_FOUND;
  UmiEditorSourceLocation departure, destination;
  UmiStatus captured = NavigationCapture(coordinator, &departure);
  if (captured != UMI_STATUS_OK && captured != UMI_STATUS_NOT_FOUND)
    return captured;
  status = umi_editor_source_location_initialize(&destination, bookmark.view_id, bookmark.line,
                                                 bookmark.column);
  if (status != UMI_STATUS_OK)
    return status;
  ++coordinator->navigation_depth;
  status = NavigationApply(coordinator, &destination);
  --coordinator->navigation_depth;
  if (status == UMI_STATUS_OK)
    NavigationRecord(coordinator, &departure, captured);
  return status;
}
/* Removing a closed marker is useful even when its document no longer exists.
 * It changes only session metadata; no source, selection or file is discarded. */
UmiStatus UmiDocumentCoordinatorRemoveBookmark(UmiDocumentCoordinator *coordinator, size_t index,
                                               uint64_t expected_revision) {
  UmiStatus status = BookmarkReady(coordinator, expected_revision);
  if (status != UMI_STATUS_OK)
    return status;
  if (index >= coordinator->bookmark_count)
    return UMI_STATUS_NOT_FOUND;
  if (coordinator->bookmark_revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memmove(&coordinator->bookmarks[index], &coordinator->bookmarks[index + 1U],
          (coordinator->bookmark_count - index - 1U) * sizeof coordinator->bookmarks[0]);
  --coordinator->bookmark_count;
  memset(&coordinator->bookmarks[coordinator->bookmark_count], 0, sizeof coordinator->bookmarks[0]);
  ++coordinator->bookmark_revision;
  return UMI_STATUS_OK;
}
