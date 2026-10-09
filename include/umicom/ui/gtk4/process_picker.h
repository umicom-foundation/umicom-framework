/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/process_picker.h
 * PURPOSE: Choose captured processes through asynchronous Framework discovery and an explicit host callback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_PROCESS_PICKER_H
#define UMICOM_UI_GTK4_PROCESS_PICKER_H
#include "umicom/desktop_system/process_catalog.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef UmiStatus (*UmiGtk4ProcessChoose)(void *context,
                                              const UmiDesktopSystemProcess *process);
    typedef UmiStatus (*UmiGtk4ProcessCapture)(void *context,
                                               const UmiDesktopProcessCaptureOptions *options,
                                               UmiDesktopProcessCatalog **out);
    typedef struct UmiGtk4ProcessProvider
    {
        UmiGtk4ProcessCapture capture;
        void *context;
        GDestroyNotify destroy;
    } UmiGtk4ProcessProvider;
    /** Create an idle process chooser. Refresh starts one cancellable worker capture.
 * NULL provider selects the native provider. A custom provider receives cancellation
 * options and must return an owned catalogue or NULL on failure. It may not use GTK.
 * Its context is retained independently of the panel until any worker completes;
 * its destroy callback must be safe on whichever thread releases the final reference.
 * On success, construction takes ownership of both contexts. On failure neither
 * destroy callback runs. The choose callback runs on the GTK thread, with a copied
 * row, only while mapped; it must enforce the host's workspace/permission policy.
 * Selection never attaches, starts or signals a process. Fixture rows and unknown
 * creation identities are displayed but cannot be chosen. Positive int32 PIDs only
 * are selectable for debugger compatibility. Recheck identity before attaching.
 * Search operates on captured names and decimal PIDs, not the current process table.
 * A pid:123 query selects an exact decimal PID instead of a substring. */
    GtkWidget *UmiGtk4ProcessPickerCreate(const UmiGtk4ProcessProvider *provider,
                                          UmiGtk4ProcessChoose choose, void *context,
                                          GDestroyNotify destroy);
#ifdef __cplusplus
}
#endif
#endif
