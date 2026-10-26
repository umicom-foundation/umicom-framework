/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/ctest_comparison.h
 *
 * PURPOSE:
 *   Expose read-only saved-run outcome comparison through native GTK controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_UI_GTK4_CTEST_COMPARISON_H
#define UMICOM_UI_GTK4_CTEST_COMPARISON_H
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Floating widget, used on the GTK main thread. Open captures two absolute
 * filenames and compares saved JUnit reports on a worker. No test executes and
 * no live test state is changed. Failed/cancelled candidates retain accepted
 * results. Hiding before admission cancels publication; once admitted, owned
 * source labels finish updating even if a text observer closes the host.
 * Every page and clipboard copy includes byte counts and SHA-256 identities
 * captured from parsed input. Paging does not reread paths; digests do not
 * authenticate a signer or prove a source revision. Failed replacements retain
 * the accepted identities along with the accepted results.
 * The host must not destroy or replace individual panel controls. */
GtkWidget *UmiGtk4CtestComparisonCreate(void);
/* True while a worker or guarded GTK publication is in progress. Cancellation
 * suppresses publication but does not interrupt an operating-system file read. */
gboolean UmiGtk4CtestComparisonPending(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
