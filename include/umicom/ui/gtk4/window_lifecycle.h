/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/window_lifecycle.h
 *
 * PURPOSE:
 *   Share native window-removal observation and checked owned-window presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_UI_GTK4_WINDOW_LIFECYCLE_H
#define UMICOM_UI_GTK4_WINDOW_LIFECYCLE_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Called once when GTK removes the window from its toplevel model, including
 * gtk_window_destroy while someone retains a reference. Runs on the GTK main
 * thread with live window and receiver references during the call. */
typedef void (*UmiGtk4WindowClosedCallback)(GtkWidget *window, gpointer data);

/** Return true for a live window in GTK's toplevel model, even if it has never
 * been presented. A valid retained but destroyed window returns false. NULL
 * returns false. The argument must be NULL or a valid GObject, never freed
 * memory. Call only on the GTK main thread. */
gboolean UmiGtk4WindowIsOpen(GtkWindow *window);

/** Observe native removal through Framework's existing weak watch. This does
 * not retain window or receiver. Destroying receiver cancels the observation
 * without calling back. data is borrowed and must remain valid until removal
 * or receiver finalisation. The caller normally stores it on receiver.
 * An already removed, retained window calls back before this returns.
 * Disconnects before calling back, so callbacks can close another window.
 * Call only on the GTK main thread. Invalid arguments return INVALID_ARGUMENT.
 * Uses GLib allocation, following the existing adapter allocation policy. */
UmiStatus UmiGtk4WatchWindowClosed(GtkWindow *window, GObject *receiver,
    UmiGtk4WindowClosedCallback callback, gpointer data);

/** Present an existing child whose transient owner is exactly owner. Both
 * objects must be live GtkWindows on the GTK main thread; this creates no
 * window, changes no transient relation and does not decide modal policy.
 * References are held across synchronous GTK observers. A nested presentation
 * of the same child returns BUSY. Removed windows, a changed owner relation,
 * or a child hidden/removed by an observer return CANCELLED. OK means the
 * child remains registered and visible on return, not that the desktop window
 * manager granted focus. Invalid objects, self-ownership and a mismatched
 * initial transient owner return INVALID_ARGUMENT. */
UmiStatus UmiGtk4PresentOwnedWindow(GtkWindow *owner, GtkWindow *child);

/** Hide an existing transient child without destroying it. Admission and live
 * owner requirements match UmiGtk4PresentOwnedWindow. Both operations share a
 * per-child guard: synchronous hide/present reentry returns BUSY. OK means the
 * owner and child remain registered, their relation is unchanged and the child
 * remains hidden after observers return. An observer that removes, reparents or
 * shows the child causes CANCELLED. Already-hidden children are accepted.
 * Applications must still recheck their own runtime token before dispatch. */
UmiStatus UmiGtk4HideOwnedWindow(GtkWindow *owner, GtkWindow *child);

/** Select the existing notebook page containing target, identified by widget
 * identity rather than a cached page index. Call on the GTK main thread with
 * valid objects. The notebook must belong to owner and target must be a page
 * or a descendant of one. Hidden pages are UNAVAILABLE; no visibility flags,
 * hierarchy, focus, transient relations or application data are changed.
 *
 * Strong references survive synchronous page-change observers. Nested calls
 * on the same notebook return BUSY. OK means the same live owner still owns
 * the notebook and target, and the same visible page remains selected on
 * return. Reordering that page is allowed; removal, reparenting, hiding or a
 * competing selection returns CANCELLED. An already destroyed owner also
 * returns CANCELLED. Invalid objects or initial ancestry return INVALID_ARGUMENT.
 * Applications must separately recheck their own runtime/session identity. */
UmiStatus UmiGtk4NotebookSelectOwnedTarget(GtkWindow *owner,
    GtkNotebook *notebook, GtkWidget *target);

#ifdef __cplusplus
}
#endif
#endif
