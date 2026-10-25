/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/kanban_binding.h
 *
 * PURPOSE:
 *   Expose explicit remembered project-board locations through native shared controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_KANBAN_BINDING_H
#define UMICOM_UI_GTK4_KANBAN_BINDING_H
#include "umicom/ui/gtk4/kanban_project.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Add explicit Remember and Recall controls to a project board window.
 * Both operate asynchronously on copied paths while no board document is open.
 * Recall fills folder and filename only if neither changed during loading; it
 * never opens an archive. settings_directory must already exist. This call
 * checks syntax only. GTK owner thread; keep window referenced. General windows
 * are refused. Repeat enable is ALREADY_EXISTS; removed windows are CANCELLED.
 * Keep location settings outside public repositories and published websites. */
UmiStatus UmiKanbanGtk4EnableProjectStorage(GtkWindow *window, const char *settings_directory);
/** Resolve the application's configuration folder through shared path services,
 * with an optional absolute portable base. Does not create any directory. */
UmiStatus UmiKanbanGtk4EnableProjectStorageForApplication(GtkWindow *window,
                                                          const char *application_name,
                                                          const char *base_override);
#ifdef __cplusplus
}
#endif
#endif
