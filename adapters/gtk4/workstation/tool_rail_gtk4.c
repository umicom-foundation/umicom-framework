/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/tool_rail_gtk4.c
 *
 * PURPOSE:
 *   Render reusable vertical or horizontal workstation activity/tool rails for app-specific surface launchers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/tool_rail.h"

GtkWidget *umi_gtk4_ws_tool_rail_create(GtkOrientation orientation) {
    GtkWidget *rail = gtk_box_new(orientation, 4);
    gtk_widget_add_css_class(rail, "umicom-tool-rail");
    return rail;
}

GtkWidget *umi_gtk4_ws_tool_rail_append(GtkWidget *rail, const char *icon_name, const char *tooltip, gboolean active) {
    GtkWidget *button;
    if (rail == NULL || !GTK_IS_BOX(rail)) return NULL;
    button = gtk_toggle_button_new();
    if (icon_name != NULL && icon_name[0] != '\0') gtk_button_set_icon_name(GTK_BUTTON(button), icon_name);
    if (tooltip != NULL) gtk_widget_set_tooltip_text(button, tooltip);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), active);
    gtk_widget_add_css_class(button, "umicom-tool-rail-item");
    gtk_box_append(GTK_BOX(rail), button);
    return button;
}
