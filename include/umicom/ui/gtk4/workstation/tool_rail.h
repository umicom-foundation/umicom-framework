/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/tool_rail.h
 *
 * PURPOSE:
 *   Render reusable vertical or horizontal workstation activity/tool rails for app-specific surface launchers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_TOOL_RAIL_H
#define UMICOM_UI_GTK4_WORKSTATION_TOOL_RAIL_H

#include <gtk/gtk.h>

/**
 * Initialise gtk4 ws tool rail from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_tool_rail_create(GtkOrientation orientation);
/**
 * Add gtk4 ws tool rail only after its inputs and available capacity have been checked.
 */
GtkWidget *umi_gtk4_ws_tool_rail_append(GtkWidget *rail, const char *icon_name, const char *tooltip, gboolean active);

/**
 * Append a named edge tab and return its rail-owned GtkToggleButton. The caller
 * connects selection handling; this helper does not change workspace state.
 * Vertical captions rotate the whole line clockwise, rather than stacking
 * letters. Both orientations inherit the theme font and foreground colour and
 * ellipsize captions beyond 240 logical pixels. The full UTF-8 title remains
 * available through the tooltip and accessible label. Invalid rails, empty
 * titles and invalid UTF-8 return NULL without adding a child.
 */
GtkWidget *umi_gtk4_ws_tool_rail_append_label(
    GtkWidget *rail, const char *title, gboolean vertical, gboolean active);

#endif
