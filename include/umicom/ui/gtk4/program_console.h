/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/program_console.h
 * PURPOSE: Let native frontends host the shared interactive program console.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_PROGRAM_CONSOLE_H
#define UMICOM_UI_GTK4_PROGRAM_CONSOLE_H
#include "umicom/terminal/program_console.h"
#include <gtk/gtk.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiGtk4ProgramConsoleCallbacks
    {
        /* Called on Run only. Check project identity, trust and competing jobs;
     * return a newly owned, unstarted console. Failure must leave out NULL. */
        UmiStatus (*prepare)(UmiProgramConsole **out, void *context);
        /* Recheck authority before input and at each UI poll. False requests Stop;
     * this is cooperative revocation, not instantaneous process isolation. */
        bool (*authorized)(void *context);
        void *context;
        GDestroyNotify destroy_context;
    } UmiGtk4ProgramConsoleCallbacks;
    /** Create a floating panel with Run, input, Send line, End input and Stop.
 * Nothing launches until Run is clicked. Context ownership transfers on success
 * and ends with panel destruction. Worker data never borrows host context.
 * Unmapping/closing stops pending work; a retained control cannot restart it.
 * Text input adds one LF byte and is not written to history. This pipe console
 * does not implement terminal escape sequences, screen control or a PTY.
 */
    UmiStatus UmiGtk4ProgramConsolePanelCreate(const UmiGtk4ProgramConsoleCallbacks *callbacks,
                                               GtkWidget **out);
    /** Observe whether the panel still owns a pending worker. GTK thread only.
 * A Stop request stays pending until native cleanup and main-context completion.
 */
    bool UmiGtk4ProgramConsolePanelPending(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
