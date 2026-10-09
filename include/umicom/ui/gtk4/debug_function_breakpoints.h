/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/debug_function_breakpoints.h
 * PURPOSE: Provide a reusable editor for session-owned function breakpoints.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DEBUG_FUNCTION_BREAKPOINTS_H
#define UMICOM_UI_GTK4_DEBUG_FUNCTION_BREAKPOINTS_H
#include "umicom/debug_runtime/function_breakpoint_session.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef UmiStatus (*UmiGtk4FunctionBreakpointsRead)(void *context,
                                                        UmiDebugFunctionSnapshot *out);
    typedef UmiStatus (*UmiGtk4FunctionBreakpointsApply)(void *context,
                                                         const UmiDebugFunctionDraft *draft);
    /** Create a floating function-breakpoint editor with explicit Refresh and Apply.
 * Read supplies an owned snapshot of the current session. Apply authorizes and
 * publishes a complete edited draft; hosts check workspace trust and lifetime.
 * No callback runs during construction. The panel owns context after success,
 * releasing it through destroy. Callbacks run synchronously on the GTK thread.
 * Add/remove/toggle/field edits stay local until Apply. Refresh discards them.
 * Hidden controls cannot publish a draft, and retained controls have no live
 * callback after the panel is released. No disk persistence is performed. */
    GtkWidget *UmiGtk4DebugFunctionBreakpointsCreate(UmiGtk4FunctionBreakpointsRead read,
                                                     UmiGtk4FunctionBreakpointsApply apply,
                                                     void *context, GDestroyNotify destroy);
#ifdef __cplusplus
}
#endif
#endif
