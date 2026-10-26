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
#include "umicom/bank_feed/statement_archive.h"
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
/* Preserve caller-supplied source format/digest in reusable review exports.
 * source may be NULL. Invalid provenance refuses creation. Original constructor
 * remains equivalent to this call with unknown source. Editing any comparison
 * field revokes aggregate, daily and reusable exports together. */
GtkWindow *UmiBankStatementGtk4CreateWithSource(const UmiBankFeed *feed, const char *account,
    const char *currency, const UmiBankFeedArchiveInfo *source, GtkWindow *parent);
/* Copy an immutable saved review into a new unpresented editor and recalculate.
 * The archive can be destroyed immediately. All original imported accounts are
 * retained. No saved success flag, approval or financial posting is accepted.
 * Native export is additionally bounded by the shared text export character
 * limit; an arithmetic result may remain available when a large export cannot. */
GtkWindow *UmiBankStatementGtk4CreateFromArchive(const UmiBankStatementArchive *archive,
    GtkWindow *parent);
#ifdef __cplusplus
}
#endif
#endif
