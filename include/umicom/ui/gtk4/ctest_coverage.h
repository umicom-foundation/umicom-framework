/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/ctest_coverage.h
 *
 * PURPOSE:
 *   Expose saved host-coverage review through an independently owned native panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_CTEST_COVERAGE_H
#define UMICOM_UI_GTK4_CTEST_COVERAGE_H
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Floating widget, parent or sink the result. All calls belong to the GTK main
 * thread. Read an absolute CSV manifest and its saved reports on a worker.
 * Open captures both path and expected source text. Later entry edits do not
 * change that request. No process, network operation or report write occurs.
 * Accepted evidence stays visible after failed/cancelled replacement. Hiding
 * cancels publication of a pending load; it does not interrupt an active file
 * read. The worker retains the panel until owner-thread completion.
 *
 * Source IDs are supplied metadata, not attestations. The panel evaluates only
 * listed requirements, never certifies the current checkout or all tests. */
/* A text notification occurs after acceptance. Hiding or cancelling from that
 * notification does not roll back the accepted review: owned filename/status
 * labels complete on the GTK thread, including when the host is retired. */
GtkWidget *UmiGtk4CtestCoverageCreate(void);
gboolean UmiGtk4CtestCoveragePending(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
