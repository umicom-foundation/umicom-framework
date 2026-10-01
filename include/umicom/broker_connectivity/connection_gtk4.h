/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/connection_gtk4.h
 * PURPOSE: Present explicit Paper and Live connection controls without order execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Paper/Live connection controls. This window never offers order execution.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IBKR_CONNECTION_GTK4_H
#define UMICOM_IBKR_CONNECTION_GTK4_H
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* GTK-main-context only. The toplevel is initially unpresented and owns its
 * connection. gtk_window_destroy closes the socket immediately, even if a
 * caller retains a reference to the destroyed window or one of its children. */
GtkWindow *UmiIbkrGtkCreate(GtkWindow *parent);
/* Wrap an existing unparented widget without changing its original owner or
 * content. The button keeps only a weak parent reference. No connection opens
 * until the user explicitly chooses Connect in the separate window. */
GtkWidget *UmiIbkrGtkWrap(GtkWidget *child, GtkWindow *parent);
#ifdef __cplusplus
}
#endif
#endif
