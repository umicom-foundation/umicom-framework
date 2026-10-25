/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/kanban_dock.h
 *
 * PURPOSE:
 *   Embed one existing board document without creating another model or file lease.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_KANBAN_DOCK_H
#define UMICOM_UI_GTK4_KANBAN_DOCK_H
#include "umicom/base/status.h"
#include "umicom/ui/gtk4/kanban_window.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Move the existing board window's contents into a new owned, nonfloating
 * pane. Both windows must be registered with the same GtkApplication; host
 * and board must differ. No file is opened, closed or saved. A pending board
 * worker refuses docking. One board can have only one dock. Failure clears out.
 * Keep both windows referenced through this call; GTK owner thread only.
 * Layout hiding/reparenting preserves the document. When host is removed or
 * pane is finalized, contents return to the independent board window and it
 * is presented, preserving unsaved work. Explicit board destruction retains
 * the established forced-teardown behavior and invalidates the dock.
 * A host that terminates its entire process still cannot retain unsaved memory. */
UmiStatus UmiKanbanGtk4DockWindow(GtkWindow *host, GtkWindow *board, GtkWidget **out);
/** Return contents to their existing window, optionally presenting it. This
 * consumes this dock's association; the pane remains a harmless placeholder.
 * Returns NOT_FOUND after return, CANCELLED after board removal, or BUSY during
 * a native transfer. Keep pane referenced. No document I/O or discard occurs. */
UmiStatus UmiKanbanGtk4Undock(GtkWidget *pane, bool present);
/** Present an existing board, returning any docked contents first. Ordinary
 * board launchers should use this instead of presenting a possibly empty
 * backing window. Pending file work continues under its existing owner. */
UmiStatus UmiKanbanGtk4PresentWindow(GtkWindow *board);
/** Query only. True means a pane still holds this window's contents. */
bool UmiKanbanGtk4WindowDocked(GtkWindow *board);
#ifdef __cplusplus
}
#endif
#endif
