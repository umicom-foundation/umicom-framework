/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/bank_statement.h
 * PURPOSE: Open an independent statement review using shared exact financial rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_BANK_STATEMENT_H
#define UMICOM_UI_GTK4_BANK_STATEMENT_H
#include "umicom/bank_feed/statement.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Create an unpresented review window for one existing account/currency.
 * Copies the imported feed; later source edits do not change this snapshot.
 * Parent may be NULL; a supplied parent must be an open native window.
 * Closing the parent closes the review. No bank connection or posting occurs.
 * GTK owning thread only. Returns NULL for invalid input or allocation failure. */
/* Daily booked balances and the separate daily CSV share this owned snapshot.
 * Editing dates or balances revokes both exports. The daily minimum includes
 * the supplied opening; it does not represent intraday lows or a live balance. */
GtkWindow *UmiBankStatementGtk4Create(const UmiBankFeed *feed, const char *account,
                                      const char *currency, GtkWindow *parent);
#ifdef __cplusplus
}
#endif
#endif
