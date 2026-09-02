/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/status_strip_gtk4.c
 *
 * PURPOSE:
 *   Render semantic status-strip items with left/right alignment for IDE, trading, media and operations workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/status_strip.h"

/*
 * Initialise gtk4 ws status strip from caller-provided values so later operations receive
 * a known state.
 */
GtkWidget *umi_gtk4_ws_status_strip_create(const UmiWsStatusStrip *strip) {
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *left = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *right = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    size_t i;
    gtk_widget_add_css_class(root, "umicom-status-strip");
    gtk_widget_set_hexpand(left, TRUE);
    gtk_box_append(GTK_BOX(root), left);
    gtk_box_append(GTK_BOX(root), right);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip == NULL) return root;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < strip->count; ++i) {
        GtkWidget *label = gtk_label_new(strip->items[i].text);
        gtk_widget_add_css_class(label, "umicom-status-item");
        gtk_box_append(GTK_BOX(strip->items[i].align_right ? right : left), label);
    }
    return root;
}
