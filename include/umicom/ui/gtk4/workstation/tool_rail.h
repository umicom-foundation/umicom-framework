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

#endif
