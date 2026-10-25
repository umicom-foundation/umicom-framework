/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/debug_console.h
 *
 * PURPOSE:
 *   Present explicit filtered debugger-output captures with stable paging.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DEBUG_CONSOLE_H
#define UMICOM_UI_GTK4_DEBUG_CONSOLE_H
#include "umicom/debug/console_report.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Host callback on the GTK owner thread. Check current workspace/authority and
 * return one owned report from the requested filter, or NULL on failure. The
 * panel owns a successful result and destroys any result returned with failure.
 * No callback occurs while merely editing filters, paging or copying. */
typedef UmiStatus (*UmiGtk4DebugConsoleQuery)(void *context, const UmiDebugConsoleFilter *filter,
                                              UmiDebugConsoleReport **out);
/** Create a floating panel. Successful construction owns context and calls
 * destroy at finalization. Refresh captures output; Previous/Next use that same
 * capture, 64 rows per page. Draft filters apply only on the next Refresh.
 * Copy page is explicit and requires mapped live controls. Clear view only
 * forgets this capture, never clears the debugger's source registry.
 * A failed refresh clears older displayed output. No expression input, DAP
 * execution, automatic refresh, network publication or disk persistence exists. */
GtkWidget *UmiGtk4DebugConsoleCreate(UmiGtk4DebugConsoleQuery query, void *context,
                                     GDestroyNotify destroy);
#ifdef __cplusplus
}
#endif
#endif
