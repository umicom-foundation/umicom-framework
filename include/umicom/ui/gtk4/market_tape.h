/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/market_tape.h
 *
 * PURPOSE:
 *   Present an independent linked market-tape practice window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_MARKET_TAPE_H
#define UMICOM_UI_GTK4_MARKET_TAPE_H
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* GTK main-context operations only. No real market-data provider, orders,
 * persistence, timers or background workers are started. Each created window
 * owns an independent fictional practice tape; destroy closes that instance.
 * The returned window follows GTK top-level ownership (gtk_window_destroy). */
GtkWindow *UmiMarketTapeGtkCreate(GtkWindow *parent);
/* Consume an unparented content widget into a thin entry-point wrapper.
 * Already-parented content is returned unchanged. The existing content remains
 * the owner's workspace; this wrapper never changes its trading state. */
GtkWidget *UmiMarketTapeGtkWrap(GtkWidget *content, GtkWindow *parent);
/**
 * @brief Create a compact entry button for the linked practice tape.
 * @param parent Borrowed native window that owns any opened practice window.
 * @return Floating GTK button, or NULL for an invalid parent.
 * @details Call on the GTK thread and parent the returned button. Retaining the
 * button after its native owner closes does not permit another window to open.
 */
GtkWidget *UmiMarketTapeGtkLauncherCreate(GtkWindow *parent);
#ifdef __cplusplus
}
#endif
#endif
