/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/window_lifecycle.h
 * PURPOSE: Share native window-removal observation across Framework and applications.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
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
#ifdef __cplusplus
}
#endif
#endif
