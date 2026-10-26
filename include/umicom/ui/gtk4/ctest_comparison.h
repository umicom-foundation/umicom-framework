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
 * The host must not destroy or replace individual panel controls. */
GtkWidget *UmiGtk4CtestComparisonCreate(void);
/* True while a worker or guarded GTK publication is in progress. Cancellation
 * suppresses publication but does not interrupt an operating-system file read. */
gboolean UmiGtk4CtestComparisonPending(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
