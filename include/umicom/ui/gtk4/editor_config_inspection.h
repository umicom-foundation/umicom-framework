/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/editor_config_inspection.h
 *
 * PURPOSE:
 *   Present asynchronous read-only project configuration review with captured paths and owned
 * lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_EDITOR_CONFIG_INSPECTION_H
#define UMICOM_UI_GTK4_EDITOR_CONFIG_INSPECTION_H
#include "umicom/base/status.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Create an application-owned, initially hidden review window on the GTK
 * thread. Supplied UTF-8 fields are copied (NULL means blank). No reads start
 * until the user chooses Inspect. Work runs serially on a worker and Close
 * requests cancellation. Busy includes worker retirement after window removal.
 * The report identifies the captured paths and is read-only: no editor
 * preferences, source files or public exports change. Keep a reference while
 * polling; destroying a window is permitted and never publishes late results.
 * NULL indicates invalid arguments or a window removed during construction. */
GtkWindow *UmiGtk4EditorConfigInspectionCreateWindow(GtkApplication *application,
                                                     const char *project_root,
                                                     const char *relative_file);
int UmiGtk4EditorConfigInspectionBusy(GtkWindow *window);
/* Last completed status; initially UNAVAILABLE. During work return BUSY.
 * Removed windows return CANCELLED. Outputs are not borrowed. */
UmiStatus UmiGtk4EditorConfigInspectionStatus(GtkWindow *window);
#ifdef __cplusplus
}
#endif
#endif
