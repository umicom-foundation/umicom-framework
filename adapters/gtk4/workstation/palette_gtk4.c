/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/palette_gtk4.c
 *
 * PURPOSE:
 *   Render searchable reusable component/tool palettes for visual design, media, kitchen and application-builder workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/palette.h"

GtkWidget *umi_gtk4_ws_palette_create(const UmiWsPaletteModel *model) {
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *search = gtk_search_entry_new();
    GtkWidget *list = gtk_list_box_new();
    size_t i;
    gtk_widget_add_css_class(root, "umicom-palette");
    gtk_entry_set_placeholder_text(GTK_ENTRY(search), "Search components…");
    gtk_box_append(GTK_BOX(root), search);
    gtk_widget_set_vexpand(list, TRUE);
    gtk_box_append(GTK_BOX(root), list);
    if (model == NULL) return root;
    for (i = 0U; i < model->count; ++i) {
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
        GtkWidget *label = gtk_label_new(model->items[i].label);
        GtkWidget *category = gtk_label_new(model->items[i].category);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(category), 0.0F);
        gtk_widget_add_css_class(category, "dim-label");
        gtk_box_append(GTK_BOX(row), label);
        gtk_box_append(GTK_BOX(row), category);
        gtk_list_box_append(GTK_LIST_BOX(list), row);
    }
    return root;
}
