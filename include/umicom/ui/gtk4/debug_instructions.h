/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/debug_instructions.h
 * PURPOSE: Present explicit paused-frame instruction pages through host-owned debugger callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DEBUG_INSTRUCTIONS_H
#define UMICOM_UI_GTK4_DEBUG_INSTRUCTIONS_H
#include "umicom/debug_runtime/instruction_inspection.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef UmiStatus (*UmiGtk4InstructionCapture)(void *context, UmiDebugInstructionContext *out);
    typedef UmiStatus (*UmiGtk4InstructionRead)(void *context,
                                                const UmiDebugInstructionContext *expected,
                                                int64_t byte_offset, int64_t instruction_offset,
                                                uint32_t count, int resolve_symbols,
                                                UmiDebugInstructionPage *out);
    /** Create an idle instruction viewer. Refresh explicitly captures a paused frame
 * then requests its first page. Read page, Earlier and Later reuse that capture.
 * Callbacks run on the GTK thread and must check host authority and stopped-state
 * ownership. A callback may run a bounded native request; no worker accesses GTK
 * or debugger owners. The panel never evaluates expressions or follows source paths.
 * Changing any request setting clears displayed instructions; failures clear the
 * page and require Refresh. Hidden/retained controls cannot send requests.
 * Successful construction owns context until finalization, calling destroy once.
 * On construction failure, context remains owned by the caller. */
    GtkWidget *UmiGtk4DebugInstructionsCreate(UmiGtk4InstructionCapture capture,
                                              UmiGtk4InstructionRead read, void *context,
                                              GDestroyNotify destroy);
#ifdef __cplusplus
}
#endif
#endif
