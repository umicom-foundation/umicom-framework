/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/focus_cycle.h
 *
 * PURPOSE:
 *   Provide keyboard navigation between existing native workspace regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_FOCUS_CYCLE_H
#define UMICOM_UI_GTK4_WORKSTATION_FOCUS_CYCLE_H
#include <gtk/gtk.h>
G_BEGIN_DECLS

/* Call on the GTK thread. Mark a panel body or a notebook, not each control.
 * A marked region owns one stop; nested marked descendants are not separate
 * stops. Unmapped notebook pages and insensitive/hidden regions are skipped.
 * No widget ownership, accessible name, visibility or layout is changed. */
void UmiGtk4FocusCycleMarkRegion(GtkWidget *region, gboolean enabled);

/* Install once on a stable workspace container. F6 moves to the next region;
 * Shift+F6 moves backwards. Other modifiers are left to application commands.
 * The bubble-phase controller respects shortcuts handled by a focused child.
 * The widget owns the controller; no separate teardown is required.
 * Call again after rebuilding layout containers to bind new GtkPaned F6
 * actions; panel bodies and nested workspaces remain separate boundaries. */
void UmiGtk4FocusCycleInstall(GtkWidget *workspace);

/* Use the same operation from an application command. Regions follow current
 * widget-tree order, wrap once, and stay inside the workspace's native root.
 * Returns TRUE only when focus ends in another eligible region. A single
 * already-focused region is left alone. Reentrant requests return FALSE.
 * Focus callbacks may redirect focus or retire the workspace; navigation
 * then stops without restoring stale focus or activating any command. */
gboolean UmiGtk4FocusCycleAdvance(GtkWidget *workspace, gboolean reverse);
G_END_DECLS
#endif
