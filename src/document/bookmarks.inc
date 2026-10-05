/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/bookmarks.inc
 * PURPOSE: Resolve session bookmarks using stable working-copy identities and existing navigation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Resolve both identities so a reopened file cannot inherit a closed tab's marker. */
static int BookmarkOpen(const UmiDocumentCoordinator *coordinator, const UmiDocumentBookmark *bookmark)
{
    for (size_t index = 0U; index < coordinator->count; ++index)
        if (coordinator->entries[index].document_id == bookmark->document_id &&
            strcmp(coordinator->entries[index].view_id, bookmark->view_id) == 0) return 1;
    return 0;
}
/* A copied summary lets hosts display counts without borrowing the array. */
UmiStatus UmiDocumentCoordinatorBookmarks(const UmiDocumentCoordinator *coordinator,
    UmiDocumentBookmarksSnapshot *out_snapshot)
{
    if (coordinator == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDocumentBookmarksSnapshot snapshot = {0};
    snapshot.count = coordinator->bookmark_count;
    snapshot.revision = coordinator->bookmark_revision;
    snapshot.busy = coordinator->save_in_progress || coordinator->reopen_in_progress || coordinator->navigation_depth != 0U;
    for (size_t index = 0U; index < snapshot.count; ++index)
        if (BookmarkOpen(coordinator, &coordinator->bookmarks[index])) ++snapshot.open_count;
    *out_snapshot = snapshot; return UMI_STATUS_OK;
}
/* Keep row ownership inside the coordinator; expose closed rows for review. */
UmiStatus UmiDocumentCoordinatorBookmarkAt(const UmiDocumentCoordinator *coordinator,
    size_t index, UmiDocumentBookmark *out_bookmark)
{
    if (coordinator == NULL || out_bookmark == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= coordinator->bookmark_count) return UMI_STATUS_NOT_FOUND;
    UmiDocumentBookmark bookmark = coordinator->bookmarks[index];
    bookmark.open = BookmarkOpen(coordinator, &bookmark);
    *out_bookmark = bookmark; return UMI_STATUS_OK;
}
/* Metadata decisions share the existing save and navigation exclusion boundary. */
static UmiStatus BookmarkReady(UmiDocumentCoordinator *coordinator, uint64_t revision)
{
    UmiDocumentBookmarksSnapshot snapshot;
    UmiStatus status = UmiDocumentCoordinatorBookmarks(coordinator, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.busy) return UMI_STATUS_BUSY;
    return snapshot.revision == revision ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
/* Capture the current full draft location, then publish one bounded metadata edit. */
UmiStatus UmiDocumentCoordinatorToggleBookmark(UmiDocumentCoordinator *coordinator, int *out_added)
{
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = BookmarkReady(coordinator, coordinator->bookmark_revision);
    if (status != UMI_STATUS_OK) return status;
    if (coordinator->bookmark_revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiEditorSourceLocation location;
    status = NavigationCapture(coordinator, &location);
    if (status != UMI_STATUS_OK) return status;
    size_t active = active_index(coordinator);
    UmiDocumentId id = coordinator->entries[active].document_id;
    for (size_t index = 0U; index < coordinator->bookmark_count; ++index) {
        UmiDocumentBookmark *bookmark = &coordinator->bookmarks[index];
        if (bookmark->document_id != id || bookmark->line != location.line) continue;
        memmove(bookmark, bookmark + 1U, (coordinator->bookmark_count - index - 1U) * sizeof(*bookmark));
        --coordinator->bookmark_count;
        memset(&coordinator->bookmarks[coordinator->bookmark_count], 0, sizeof(*bookmark));
        ++coordinator->bookmark_revision;
        if (out_added != NULL) *out_added = 0;
        return UMI_STATUS_OK;
    }
    if (coordinator->bookmark_count == UMI_DOCUMENT_BOOKMARK_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentSnapshot stored;
    status = umi_document_store_snapshot(coordinator->store, id, &stored);
    if (status != UMI_STATUS_OK) return status;
    UmiDocumentBookmark bookmark = {0};
    bookmark.document_id = id; bookmark.line = location.line; bookmark.column = location.column;
    memcpy(bookmark.view_id, coordinator->entries[active].view_id, sizeof bookmark.view_id);
    memcpy(bookmark.display_name, stored.display_name, sizeof bookmark.display_name);
    coordinator->bookmarks[coordinator->bookmark_count++] = bookmark;
    ++coordinator->bookmark_revision;
    if (out_added != NULL) *out_added = 1;
    return UMI_STATUS_OK;
}
/* Search at most one pass in insertion order, using the common live resolver. */
UmiStatus UmiDocumentCoordinatorTravelBookmark(UmiDocumentCoordinator *coordinator,
    int direction, uint64_t expected_revision)
{
    if (direction != -1 && direction != 1) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = BookmarkReady(coordinator, expected_revision);
    if (status != UMI_STATUS_OK) return status;
    size_t count = coordinator->bookmark_count;
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
    UmiEditorSourceLocation departure;
    UmiStatus captured = NavigationCapture(coordinator, &departure);
    if (captured != UMI_STATUS_OK && captured != UMI_STATUS_NOT_FOUND) return captured;
    size_t current = SIZE_MAX;
    if (captured == UMI_STATUS_OK)
        for (size_t index = 0U; index < count; ++index)
            if (strcmp(coordinator->bookmarks[index].view_id, departure.uri) == 0 &&
                coordinator->bookmarks[index].line == departure.line) { current = index; break; }
    for (size_t step = 1U; step <= count; ++step) {
        size_t index = direction > 0 ? (current == SIZE_MAX ? step - 1U : (current + step) % count)
            : (current == SIZE_MAX ? count - step : (current + count - step) % count);
        const UmiDocumentBookmark *bookmark = &coordinator->bookmarks[index];
        if (!BookmarkOpen(coordinator, bookmark)) continue;
        UmiEditorSourceLocation destination;
        status = umi_editor_source_location_initialize(&destination, bookmark->view_id, bookmark->line, bookmark->column);
        if (status != UMI_STATUS_OK) return status;
        /* Resolve first, then select. Missing lines cannot move focus or change
         * history, and a successful jump uses the existing Back/Forward owner. */
        ++coordinator->navigation_depth;
        status = NavigationApply(coordinator, &destination);
        --coordinator->navigation_depth;
        if (status == UMI_STATUS_NOT_FOUND) continue;
        if (status == UMI_STATUS_OK) NavigationRecord(coordinator, &departure, captured);
        return status;
    }
    return UMI_STATUS_NOT_FOUND;
}
/* Clearing bookmarks deliberately leaves document contents and history alone. */
UmiStatus UmiDocumentCoordinatorClearBookmarks(UmiDocumentCoordinator *coordinator, uint64_t expected_revision)
{
    UmiStatus status = BookmarkReady(coordinator, expected_revision);
    if (status != UMI_STATUS_OK || coordinator->bookmark_count == 0U) return status;
    if (coordinator->bookmark_revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(coordinator->bookmarks, 0, sizeof coordinator->bookmarks);
    coordinator->bookmark_count = 0U; ++coordinator->bookmark_revision;
    return UMI_STATUS_OK;
}
