/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/debug_backtrace.h
 *
 * PURPOSE:
 *   Present an owned call-stack review with explicit path display and clipboard copy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DEBUG_BACKTRACE_H
#define UMICOM_UI_GTK4_DEBUG_BACKTRACE_H
#include "umicom/debug/backtrace.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Create an application-owned review window on the GTK thread, initially hidden.
 * The capture is cloned; callers may destroy their copy immediately. Source paths
 * start omitted. Changing the dropdown only reformats the same observation.
 * Copy reviewed text is the only clipboard operation and requires a mapped live
 * review control. No file, protocol, network or debugger mutation occurs.
 * The window closes with owner. Failure sets *out=NULL. On success out is borrowed
 * from GTK; retain it explicitly if polling after destruction. */
UmiStatus UmiGtk4DebugBacktraceCreateWindow(GtkWindow *owner, const UmiDebugBacktrace *trace,
                                            GtkWindow **out);
#ifdef __cplusplus
}
#endif
#endif
