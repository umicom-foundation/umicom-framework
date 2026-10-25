/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/local_file_drop.h
 *
 * PURPOSE:
 *   Receive bounded local file lists without coupling native drops to file I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_LOCAL_FILE_DROP_H
#define UMICOM_UI_GTK4_LOCAL_FILE_DROP_H
#include "umicom/base/status.h"
#include <gtk/gtk.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LOCAL_FILE_DROP_MAXIMUM 128U
#define UMI_LOCAL_FILE_DROP_PATH_CAPACITY 4096U

/** The attached widget and copied UTF-8 absolute paths are borrowed only for
 * this callback. Copy paths before scheduling work. OK accepts the drop;
 * every other status rejects it. No existence check, open, import, move or
 * deletion is performed by the adapter. The receiver owns those decisions. */
typedef UmiStatus (*UmiLocalFileDropGtk4Receiver)(GtkWidget *widget, const char *const *paths,
                                                  size_t count, gpointer context);

/** Return a full-owned controller to pass to gtk_widget_add_controller.
 * maximumFiles is in [1, UMI_LOCAL_FILE_DROP_MAXIMUM]. A non-NULL receiver is
 * required. On success the controller owns context and calls destroy once at
 * finalization; on failure context remains caller-owned.
 *
 * GTK owner thread only. Accepts GdkFileList copy operations; every item must
 * resolve to an absolute local UTF-8 path shorter than PATH_CAPACITY bytes.
 * Empty, oversized, remote or malformed selections are rejected in full.
 * Order and duplicates are retained for the receiver's document policy.
 * Drops require a visible, sensitive widget in an open GtkWindow. Hidden,
 * detached, removed and recursively invoked controllers do not call receivers.
 * The controller holds itself and its widget during the callback, allowing
 * the receiver to detach the controller or close the window safely.
 * Destruction of an attached host never calls the receiver.
 */
GtkDropTarget *UmiLocalFileDropGtk4Create(size_t maximumFiles,
                                          UmiLocalFileDropGtk4Receiver receiver, gpointer context,
                                          GDestroyNotify destroy);
#ifdef __cplusplus
}
#endif
#endif
