/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/kanban_window.h
 * PURPOSE: Own a native planning document window reusable by Studio and standalone hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_KANBAN_WINDOW_H
#define UMICOM_UI_GTK4_KANBAN_WINDOW_H
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Creates an application-owned window on the GTK owner thread. The user chooses
 * an existing absolute folder and a .umikb filename. Native document I/O runs
 * on a serialized worker, with board controls frozen until completion.
 * Close requests refuse pending I/O and unsaved edits unless the user has
 * explicitly enabled discard. Use gtk_window_close(), not destroy(), while
 * a document is open. Process termination cannot preserve unsaved memory.
 * No network listener, account authentication or automatic publication exists. */
GtkWindow *UmiKanbanGtk4CreateWindow(GtkApplication *application);
/* Floating launch button for a native host. It must remain rooted in owner,
 * which supplies its GtkApplication. The new board window owns its document
 * independently, so closing Studio does not discard its unsaved board. */
GtkWidget *UmiKanbanGtk4CreateLauncher(GtkWindow *owner);
#ifdef __cplusplus
}
#endif
#endif
