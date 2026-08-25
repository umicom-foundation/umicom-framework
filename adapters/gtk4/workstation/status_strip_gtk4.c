/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/status_strip_gtk4.c
 *
 * PURPOSE:
 *   Render semantic status-strip items with left/right alignment for IDE, trading, media and operations workstations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/status_strip.h"

GtkWidget *umi_gtk4_ws_status_strip_create(const UmiWsStatusStrip *strip) {
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *left = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *right = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    size_t i;
    gtk_widget_add_css_class(root, "umicom-status-strip");
    gtk_widget_set_hexpand(left, TRUE);
    gtk_box_append(GTK_BOX(root), left);
    gtk_box_append(GTK_BOX(root), right);
    if (strip == NULL) return root;
    for (i = 0U; i < strip->count; ++i) {
        GtkWidget *label = gtk_label_new(strip->items[i].text);
        gtk_widget_add_css_class(label, "umicom-status-item");
        gtk_box_append(GTK_BOX(strip->items[i].align_right ? right : left), label);
    }
    return root;
}
