/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/accounting_practice.h
 *
 * PURPOSE:
 *   Expose a shared local accounting batch capture and review window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_UI_GTK4_ACCOUNTING_PRACTICE_H
#define UMICOM_UI_GTK4_ACCOUNTING_PRACTICE_H
#include <gtk/gtk.h>
#include "umicom/open_tms/accounting_book.h"
#ifdef __cplusplus
extern "C" {
#endif
/* GTK owner thread only. Create an unpresented independent empty batch.
 * Captures, approvals and posting flags belong to the independent local batch.
 * Explicit Save writes an owned capture to a new file; Open prepares a complete
 * replacement review. Closing discards unsaved edits. No bank connection,
 * authenticated approval or ledger posting occurs.
 * Historical interface: the original window was in-memory only and used no files.
 * It is retained as the default behavior until the user explicitly requests I/O.
 * NULL parent is allowed. Parent/window removal retires retained controls.
 * Present or destroy the returned GTK-owned window. */
GtkWindow *UmiGtk4AccountingPracticeCreate(GtkWindow *parent);
/* Copy the current local batch while its host is live. Supply a complete book,
 * preferably heap allocated. Refusal leaves the output unchanged.
 * Flags are local workflow state, not authenticated approval or posting receipts. */
UmiStatus UmiGtk4AccountingPracticeRead(GtkWindow *window, UmiOpenTmsAccountingBook *out_book);
/* Observe asynchronous file activity and availability of a replacement review.
 * Distinct non-null outputs are written only on success, on the GTK owner thread.
 * Closing retires the host; an already dispatched exclusive write may complete
 * against its owned capture. A loaded batch never applies itself. */
UmiStatus UmiGtk4AccountingPracticeFileState(GtkWindow *window, bool *outRunning, bool *outReady);
/* Floating launcher opens a new independent batch. A retained button without
 * a live parent window is inert. */
GtkWidget *UmiGtk4AccountingPracticeLauncherCreate(void);
#ifdef __cplusplus
}
#endif
#endif
