/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/bank_feed.h
 * PURPOSE: Open a local bank transaction review using reusable Framework services.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_BANK_FEED_H
#define UMICOM_UI_GTK4_BANK_FEED_H
#include <gtk/gtk.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Create a floating launcher for the local bank transaction inspector.
 * A click opens a window attached to the live parent. Users explicitly select
 * a saved Open Banking JSON response; loading never contacts a bank or posts
 * ledger entries. The window shows per-account/currency totals and exports
 * the imported observations. Closing retires retained controls and late loads. */
GtkWidget *UmiBankFeedGtk4LauncherCreate(void);
/** Create an unpresented bank transaction window owned by GTK. parent may be
 * NULL; a removed parent returns NULL. The caller presents it and eventually destroys it. Import
 * work runs off the GTK thread; each result belongs to the path generation that started it.
 * Changing path or clearing the view invalidates earlier work. */
GtkWindow *UmiBankFeedGtk4Create(GtkWindow *parent);
/** Return whether an import worker is still pending, including after native
 * window removal when the caller retains the window. Call on the GTK thread.
 * A destroyed result is discarded; this is not a network connection status. */
bool UmiBankFeedGtk4Busy(GtkWindow *window);
#ifdef __cplusplus
}
#endif
#endif
