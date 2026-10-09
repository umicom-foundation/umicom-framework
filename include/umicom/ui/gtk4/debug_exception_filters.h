/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/debug_exception_filters.h
 * PURPOSE: Offer reusable exception-filter controls without owning a debugger or workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DEBUG_EXCEPTION_FILTERS_H
#define UMICOM_UI_GTK4_DEBUG_EXCEPTION_FILTERS_H
#include "umicom/debug_runtime/exception_filters.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif
    /* Retain the previous callback declaration for review. Use the Framework
     * filter aggregate so host callbacks and the GTK panel share the same
     * bounded storage without changing the persisted exception-record type. */
#if 0
    typedef UmiStatus (*UmiGtk4ExceptionFiltersRead)(void *context, UmiDebugExceptionSnapshot *out);
#endif
    typedef UmiStatus (*UmiGtk4ExceptionFiltersRead)(void *context, UmiDebugExceptionFiltersSnapshot *out);
    typedef UmiStatus (*UmiGtk4ExceptionFiltersApply)(void *context,
                                                      const UmiDebugExceptionSelection *selection);
    /** Create a floating panel with explicit Refresh and Apply actions.
 * Host callbacks run on the GTK owner thread and must enforce workspace trust
 * and session ownership. The panel owns context after successful construction;
 * destroy releases it once. It neither starts nor stops a debugger.
 * Choices stay in memory and refer to the captured session revision. Hidden or
 * retained controls cannot apply them. The host must keep callbacks bounded.
 * Creation calls no callbacks; Refresh obtains the connected adapter catalogue. */
    GtkWidget *UmiGtk4DebugExceptionFiltersCreate(UmiGtk4ExceptionFiltersRead read,
                                                  UmiGtk4ExceptionFiltersApply apply, void *context,
                                                  GDestroyNotify destroy);
#ifdef __cplusplus
}
#endif
#endif
