/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/panel_frame_gtk4.c
 *
 * PURPOSE:
 *   Render Framework panel chrome around arbitrary GTK4 child content without moving panel ownership into the adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/panel_frame.h"

GtkWidget *umi_gtk4_ws_panel_frame_create(const UmiWsPanelChrome *chrome, GtkWidget *child) {
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *title = gtk_label_new(chrome != NULL ? chrome->title : "Panel");
    GtkWidget *badge = gtk_label_new(chrome != NULL ? chrome->badge : "");
    gtk_widget_add_css_class(frame, "umicom-workstation-panel");
    gtk_widget_add_css_class(header, "umicom-panel-header");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_widget_set_hexpand(title, TRUE);
    gtk_box_append(GTK_BOX(header), title);
    if (chrome != NULL && chrome->badge[0] != '\0') {
        gtk_widget_add_css_class(badge, "umicom-panel-badge");
        gtk_box_append(GTK_BOX(header), badge);
    }
    if (chrome != NULL && chrome->show_pin) gtk_box_append(GTK_BOX(header), gtk_button_new_from_icon_name("view-pin-symbolic"));
    if (chrome != NULL && chrome->show_menu) gtk_box_append(GTK_BOX(header), gtk_menu_button_new());
    if (chrome != NULL && chrome->show_close) gtk_box_append(GTK_BOX(header), gtk_button_new_from_icon_name("window-close-symbolic"));
    gtk_box_append(GTK_BOX(root), header);
    if (child != NULL) { gtk_widget_set_hexpand(child, TRUE); gtk_widget_set_vexpand(child, TRUE); gtk_box_append(GTK_BOX(root), child); }
    gtk_frame_set_child(GTK_FRAME(frame), root);
    return frame;
}
