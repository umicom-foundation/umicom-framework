/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/bookmark_panel.h
 *
 * PURPOSE:
 *   Expose session bookmark review through the shared native document binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_BOOKMARK_PANEL_H
#define UMICOM_UI_GTK4_BOOKMARK_PANEL_H
#include "umicom/document/bookmark_selection.h"
#include "umicom/ui/gtk4.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Notification after a live native action. visited is true for source navigation
 * and false for metadata removal. Products may reveal their editor here.
 * Callback context must not own a strong reference to the returned panel. */
typedef void (*UmiGtk4BookmarkPanelResult)(void *context, UmiStatus status, int visited);
/* Return an owned, nonfloating widget; parent it and release the caller's
 * reference. NULL means the adapter has no live document binding. Context and
 * destroy transfer only on successful construction. Embed below the adapter's
 * existing window. The panel retains the invalidatable edit token, never the
 * coordinator. Unbinding/retiring the adapter makes retained controls inert. */
GtkWidget *UmiGtk4BookmarkPanelCreate(UmiGtk4Adapter *adapter, UmiGtk4BookmarkPanelResult result,
                                      void *context, GDestroyNotify destroy);
/* Refresh copied session markers on the owning GTK thread. This never performs
 * filesystem I/O. A stale or busy owner refuses publication. A reentrant call
 * returns BUSY. The explicit Refresh button is always available for a live owner. */
UmiStatus UmiGtk4BookmarkPanelRefresh(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
