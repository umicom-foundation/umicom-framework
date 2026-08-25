/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/shell_header_gtk4.c
 *
 * PURPOSE:
 *   Render a reusable professional workstation shell header for Studio, Trader, media, design and system applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/shell_header.h"

GtkWidget *umi_gtk4_ws_shell_header_create(const char *title, const char *subtitle, const char *mode_badge, bool compact) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, compact ? 6 : 10);
    GtkWidget *titles = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    GtkWidget *primary = gtk_label_new(title != NULL ? title : "Umicom Workstation");
    GtkWidget *secondary = gtk_label_new(subtitle != NULL ? subtitle : "");
    GtkWidget *badge = gtk_label_new(mode_badge != NULL ? mode_badge : "");
    gtk_widget_add_css_class(box, "umicom-workstation-header");
    gtk_widget_add_css_class(primary, "title-3");
    gtk_widget_add_css_class(secondary, "dim-label");
    gtk_widget_add_css_class(badge, "umicom-mode-badge");
    gtk_label_set_xalign(GTK_LABEL(primary), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(secondary), 0.0F);
    gtk_box_append(GTK_BOX(titles), primary);
    if (subtitle != NULL && subtitle[0] != '\0') gtk_box_append(GTK_BOX(titles), secondary);
    gtk_box_append(GTK_BOX(box), titles);
    gtk_widget_set_hexpand(titles, TRUE);
    if (mode_badge != NULL && mode_badge[0] != '\0') gtk_box_append(GTK_BOX(box), badge);
    return box;
}
