/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_workspace/gtk4.h
 * PURPOSE: Present desktop workspace drafts and explicit storage actions through GTK.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_DESKTOP_WORKSPACE_GTK4_H
#define UMICOM_DESKTOP_WORKSPACE_GTK4_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Create an ordinary desktop window without opening storage. Pass an existing
 * GtkApplication for application lifetime tracking, or NULL for tests. A NULL
 * directory selects the current user's Umicom data directory. Editing and all
 * widget calls occur on the GTK thread; storage operations use one owned task. */
GtkWindow *UmiDesktopWorkspaceGtkCreate(GtkApplication *application, const char *directory);
/** Add a launcher to an existing box. No files are opened until the resulting
 * window's Open workspace action. All existing children remain in place. */
UmiStatus UmiDesktopWorkspaceGtkAttach(GtkWidget *box);
#ifdef __cplusplus
}
#endif
#endif
