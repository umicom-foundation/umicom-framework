/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workspace_library.h
 * PURPOSE: Let desktop users choose the local database that owns their work.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WORKSPACE_LIBRARY_H
#define UMICOM_UI_GTK4_WORKSPACE_LIBRARY_H
#include <gtk/gtk.h>
#include <stdbool.h>
#include "umicom/data/data_server.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkspaceLibraryGtk UmiWorkspaceLibraryGtk;
/**
 * Open a candidate library on the GTK thread. The path is borrowed for this
 * call. Return OK only after the replacement is ready; keep the previous
 * workspace on failure. The callback must not destroy its selector.
 */
typedef UmiStatus (*UmiWorkspaceLibraryOpen)(const char *path, bool create, void *context);

/**
 * Create an inert selector. It opens no files or connections until the user
 * chooses Create or Open. The callback context is borrowed until Destroy.
 */
UmiStatus UmiWorkspaceLibraryGtkCreate(UmiWorkspaceLibraryOpen open, void *context,
    UmiWorkspaceLibraryGtk **outLibrary);
/** Return the borrowed root widget; the controller owns its reference. */
GtkWidget *UmiWorkspaceLibraryGtkWidget(UmiWorkspaceLibraryGtk *library);
/** Prefill a path without opening it or changing the active library. */
void UmiWorkspaceLibraryGtkSetPath(UmiWorkspaceLibraryGtk *library, const char *path);
/** Record a successfully opened path; this performs no file operation. */
void UmiWorkspaceLibraryGtkSetCurrentPath(UmiWorkspaceLibraryGtk *library, const char *path);
/**
 * Enable recent-library metadata at an absolute UTF-8 preferences path once.
 * Loading runs on a worker and never opens a listed database or fills the draft.
 * Successful user opens and explicit history edits may create the preferences
 * parent, then replace metadata. The displayed preferences contain paths only.
 * Malformed existing metadata is reported and preserved; writes never repair it
 * silently. Calls in this process are serialized; other processes are last-writer
 * wins. All selector methods belong to the GTK thread.
 */
UmiStatus UmiWorkspaceLibraryGtkEnableHistory(UmiWorkspaceLibraryGtk *library, const char *settingsPath);
/**
 * Queue the current successfully opened location for the enabled history.
 * Used after a programmatic open or to retry a failed preference write. It never
 * opens a database. Completion status appears in the separate history message.
 */
UmiStatus UmiWorkspaceLibraryGtkRememberCurrent(UmiWorkspaceLibraryGtk *library);
/** Return whether metadata work remains; database and model work are independent. */
bool UmiWorkspaceLibraryGtkHistoryBusy(const UmiWorkspaceLibraryGtk *library);
/**
 * Cancel a pending file picker and disconnect retained controls before their
 * borrowed application context is released. An active history write may finish;
 * queued metadata work is discarded. Call once, on the GTK thread.
 */
void UmiWorkspaceLibraryGtkDestroy(UmiWorkspaceLibraryGtk *library);
/**
 * Open a selected local SQLite database. Create reserves a new file exclusively;
 * Open requires an existing regular file. Paths must be absolute UTF-8. Parent
 * directories must already exist. A failed Create can leave its newly reserved
 * file for inspection; it never deletes or overwrites an existing file. A path
 * must not be concurrently replaced by another process during this operation.
 * On failure outServer is NULL. On success the caller owns the Data Server.
 */
UmiStatus UmiWorkspaceLibraryOpenData(const char *path, bool create,
    UmiDataServer **outServer);
#ifdef __cplusplus
}
#endif
#endif
