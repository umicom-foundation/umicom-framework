/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/debug_modules.h
 * PURPOSE: Browse debugger module metadata one bounded page at a time.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DEBUG_MODULES_H
#define UMICOM_UI_GTK4_DEBUG_MODULES_H
#include "umicom/debug_runtime/module_page.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif
    /* expected is NULL only for an explicit Refresh from the first page.
 * Otherwise the host must preserve the captured connection identity. */
    typedef UmiStatus (*UmiGtk4DebugModulesQuery)(void *context,
                                                  const UmiDebugModuleSession *expected,
                                                  uint32_t first, uint32_t count,
                                                  UmiDebugModulePage *out);
    /** Create a floating metadata browser with explicit Refresh, Previous and Next.
 * Query runs on the GTK owner thread and must be bounded and workspace-authorized.
 * On success, the panel owns context until destroy. Construction performs no I/O.
 * Paths are displayed as adapter metadata; no local file is opened or executed.
 * Navigation belongs to the captured session. Hidden/retained controls cannot
 * issue requests. A failed page retires navigation until the next Refresh. */
    GtkWidget *UmiGtk4DebugModulesCreate(UmiGtk4DebugModulesQuery query, void *context,
                                         GDestroyNotify destroy);
#ifdef __cplusplus
}
#endif
#endif
