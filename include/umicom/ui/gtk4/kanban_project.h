/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/kanban_project.h
 *
 * PURPOSE:
 *   Associate a native board window with one explicit project while keeping storage independently
 * chosen.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_KANBAN_PROJECT_H
#define UMICOM_UI_GTK4_KANBAN_PROJECT_H
#include "umicom/kanban/location.h"
#include "umicom/ui/gtk4/kanban_window.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Create an independent native board window for a captured project root.
 * Suggest its deterministic filename and display the project identity. Leave
 * the storage directory empty so the user chooses private storage explicitly.
 * No archive is opened, created or published by this call. The project path is
 * window metadata, never automatically copied into cards or public exports.
 * GTK owner thread only; out_window must address NULL and is preserved on
 * failure. The GtkApplication owns the successful window. Present it separately.
 * The project association stays fixed if the host later switches workspace.
 * Native removal or edited fields during construction cancel the new window. */
UmiStatus UmiKanbanGtk4CreateProjectWindow(GtkApplication *application, const char *project_root,
                                           GtkWindow **out_window);
/** Copy the original project identity and suggested filename. This does not
 * return edited form fields or certify an open archive. A general-purpose
 * window returns NOT_FOUND; a removed project window returns CANCELLED.
 * Keep the window referenced; outputs remain unchanged on failure. */
UmiStatus UmiKanbanGtk4ProjectLocation(GtkWindow *window, UmiKanbanProjectLocation *out);
#ifdef __cplusplus
}
#endif
#endif
