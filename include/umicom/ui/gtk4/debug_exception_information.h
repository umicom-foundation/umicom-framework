/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/debug_exception_information.h
 * PURPOSE: Present captured exception details as plain text through an explicitly authorised host callback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DEBUG_EXCEPTION_INFORMATION_H
#define UMICOM_UI_GTK4_DEBUG_EXCEPTION_INFORMATION_H
#include "umicom/debug_runtime/exception_inspection.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef UmiStatus (*UmiGtk4DebugExceptionInformationQuery)(
        void *context, UmiDebugExceptionTarget *out_target,
        UmiDebugExceptionInformation *out_information);
    /** Create an idle exception viewer that requests data only on Refresh exception.
 * The host callback owns workspace and debugger authority checks on the GTK thread.
 * Returned strings are copied into plain text widgets. Expressions, markup and
 * paths are descriptive metadata; the panel never evaluates or opens them.
 * A successful construction owns context until finalization and invokes destroy once.
 * Hidden/retained controls cannot query. Displayed results are labelled as captures,
 * not a live view; failed refreshes clear the previous capture. */
    GtkWidget *UmiGtk4DebugExceptionInformationCreate(UmiGtk4DebugExceptionInformationQuery query,
                                                      void *context, GDestroyNotify destroy);
#ifdef __cplusplus
}
#endif
#endif
