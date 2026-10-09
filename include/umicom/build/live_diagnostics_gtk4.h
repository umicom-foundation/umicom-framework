/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/live_diagnostics_gtk4.h
 * PURPOSE: Present bounded live compiler records without borrowing worker state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_LIVE_DIAGNOSTICS_GTK4_H
#define UMICOM_BUILD_LIVE_DIAGNOSTICS_GTK4_H
#include "umicom/build/live_diagnostics.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif

    /** The view reports a stable producer index, not a borrowed diagnostic.
     * The host must re-read the matching operation and phase before opening it. */
    typedef UmiStatus (*UmiBuildDiagnosticSourceOpen)(uint64_t operation,
        size_t phase_index, size_t diagnostic_index, void *context);
    /** Bind an optional source action once. Context ownership transfers only on
     * success and destroy_context runs with panel destruction. Calls and callbacks
     * occur on the GTK thread. A callback may release the host or panel; the view
     * retains itself until the action returns. A second binding is refused. */
    UmiStatus UmiBuildDiagnosticViewGtk4SetSourceOpener(GtkWidget *panel,
        UmiBuildDiagnosticSourceOpen opener, void *context, GDestroyNotify destroy_context);

    /** Create an initially empty, selectable diagnostic page with previous/next
     * controls. GTK owns child controls. Retained buttons become inert when their
     * panel is destroyed. The caller owns the returned floating widget reference. */
    GtkWidget *UmiBuildDiagnosticViewGtk4Create(void);
    /** Read the requested producer page on the GTK thread. Initial identity zero
     * asks for current evidence. The host reads that page from its session; on
     * INVALID_STATE it reads the current first page to establish the new phase.
     * Outputs are mandatory and unchanged on invalid input. */
    UmiStatus UmiBuildDiagnosticViewGtk4Request(GtkWidget *panel, uint64_t *operation,
                                                size_t *phase_index, size_t *first_index);
    /** Copy and render a validated page on the GTK thread. Caller storage is not
     * retained. Older operation/phase evidence and an unsolicited page for the same
     * phase are refused unchanged. A new phase starts at its first page.
     * The view repairs malformed display UTF-8 but never interprets text as markup,
     * opens files during rendering, or describes zero errors as successful compilation. */
    UmiStatus UmiBuildDiagnosticViewGtk4Update(GtkWidget *panel,
                                               const UmiBuildDiagnosticPage *page);
#ifdef __cplusplus
}
#endif
#endif
