/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/live_diagnostics.h
 * PURPOSE: Host persistent source diagnostics with explicit launch and application-owned source capture.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_LIVE_DIAGNOSTICS_H
#define UMICOM_UI_GTK4_LIVE_DIAGNOSTICS_H
#include "umicom/language_runtime/diagnostic_monitor.h"
#include <gtk/gtk.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiGtk4LiveDiagnosticCallbacks
    {
        /* GTK thread only. Revalidate the captured document/workspace and copy its
     * current complete UTF-8 draft into out; do not truncate. No worker borrows
     * the document, callback context or this temporary buffer. */
        UmiStatus (*capture)(char *out, size_t capacity, size_t *out_bytes, void *context);
        /* Recheck explicit launch authority and workspace trust. False cancels work
     * cooperatively; the panel also stops on capture failure or unmap. */
        bool (*authorized)(void *context);
        void *context;
        GDestroyNotify destroy_context;
    } UmiGtk4LiveDiagnosticCallbacks;
    /** Create a floating panel with server/argument fields, Start, Stop and paged
 * diagnostics. Config text is copied. Context transfers only on success and is
 * destroyed with the panel; it never crosses to the protocol worker.
 * Nothing starts during creation or mapping. Start captures the latest draft.
 * A 350 ms UI poll observes edits and authorization. Hidden/closed panels stop
 * without blocking GTK. Retained controls cannot start a closed panel.
 * This view does not apply fixes, save files or open links. An optional host
 * callback can navigate the editor after an explicit Open source action.
 * It displays 32 rows per page, clipping very long messages with an explicit
 * notice; the shared catalogue retains the complete bounded publication. */
    UmiStatus UmiGtk4LiveDiagnosticPanelCreate(const UmiLanguageDiagnosticMonitorConfig *config,
                                               const UmiGtk4LiveDiagnosticCallbacks *callbacks,
                                               GtkWidget **out);
    /** Optional editor navigation callback, called only by an explicit Open source
     * action on the GTK thread. Arguments are borrowed through return and remain
     * valid if the callback hides the panel. Recheck document ownership and use
     * the supplied complete source as a freshness proof before selecting a range.
     * The context is the Create callbacks' context; no second owner is created. */
    typedef UmiStatus (*UmiGtk4LiveDiagnosticNavigate)(const char *uri, const char *source,
                                                       size_t source_bytes,
                                                       UmiEditorTextPosition start,
                                                       UmiEditorTextPosition end, void *context);
    /** Set or clear navigation before Start or after the worker has stopped.
     * BUSY preserves the existing binding during callbacks or a pending worker.
     * The panel never opens a file itself. A NULL callback disables Open source. */
    UmiStatus UmiGtk4LiveDiagnosticPanelSetNavigator(GtkWidget *panel,
                                                     UmiGtk4LiveDiagnosticNavigate navigate);
    /** GTK-thread observation, true until worker cleanup has returned to the UI. */
    bool UmiGtk4LiveDiagnosticPanelPending(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
