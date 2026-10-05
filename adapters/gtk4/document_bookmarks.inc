/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_bookmarks.inc
 * PURPOSE: Route bookmark intent through the shared document binding and completion owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/bookmarks.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
static UmiStatus NativeBookmarkReady(UmiGtk4Adapter *adapter, int action, UmiDocumentBookmarksSnapshot *snapshot)
{
    if (adapter == NULL || action < -1 || action > 2) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL) return UMI_STATUS_UNAVAILABLE;
    /* A retained native window is not necessarily an open editor. */
    if (!UmiGtk4WindowIsOpen(adapter->window)) return UMI_STATUS_UNAVAILABLE;
    if (adapter->edit_cancel != NULL || UmiGtk4AdapterDocumentSaveAllBusy(adapter) ||
        UmiGtk4AdapterDocumentCloseBusy(adapter) || UmiGtk4AdapterReplacementReviewBusy(adapter)) return UMI_STATUS_BUSY;
    UmiStatus status = UmiDocumentCoordinatorBookmarks(adapter->edit_coordinator, snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot->busy) return UMI_STATUS_BUSY;
    if (action == 0) { UmiDocumentWorkingCopySnapshot active; return umi_document_coordinator_active_snapshot(adapter->edit_coordinator, &active); }
    return (action == 2 ? snapshot->count : snapshot->open_count) != 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
int UmiGtk4AdapterBookmarkEnabled(UmiGtk4Adapter *adapter, int action)
{
    UmiDocumentBookmarksSnapshot snapshot;
    return NativeBookmarkReady(adapter, action, &snapshot) == UMI_STATUS_OK;
}
UmiStatus UmiGtk4AdapterBookmarkCommand(UmiGtk4Adapter *adapter, int action)
{
    UmiDocumentBookmarksSnapshot snapshot;
    UmiStatus status = NativeBookmarkReady(adapter, action, &snapshot);
    if (status == UMI_STATUS_OK) {
        if (action == 0) status = UmiDocumentCoordinatorToggleBookmark(adapter->edit_coordinator, NULL);
        else if (action == 2) status = UmiDocumentCoordinatorClearBookmarks(adapter->edit_coordinator, snapshot.revision);
        else status = UmiDocumentCoordinatorTravelBookmark(adapter->edit_coordinator, action, snapshot.revision);
    }
    /* Completion can destroy the application; do not access it afterwards. */
    return adapter != NULL ? EditFinished(adapter, status) : status;
}
