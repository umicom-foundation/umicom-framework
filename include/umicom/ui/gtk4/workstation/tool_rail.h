/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/tool_rail.h
 *
 * PURPOSE:
 *   Render reusable vertical or horizontal workstation activity/tool rails for app-specific surface launchers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_TOOL_RAIL_H
#define UMICOM_UI_GTK4_WORKSTATION_TOOL_RAIL_H

#include <gtk/gtk.h>

GtkWidget *umi_gtk4_ws_tool_rail_create(GtkOrientation orientation);
GtkWidget *umi_gtk4_ws_tool_rail_append(GtkWidget *rail, const char *icon_name, const char *tooltip, gboolean active);

#endif
