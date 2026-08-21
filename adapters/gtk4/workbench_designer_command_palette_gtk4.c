/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_command_palette_gtk4.c
 *
 * PURPOSE:
 *   Render the Framework designer command palette as a searchable GTK4 popover
 *   model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


static void command_palette_search_changed(GtkEditable *editable, gpointer user_data)
{
    UmiWorkbenchDesignerGtk4 *designer = user_data;
    const char *query;
    if (designer == NULL) return;
    query = gtk_editable_get_text(editable);
    (void)umi_workbench_designer_command_palette_filter(
        &designer->bundle.command_palette, query);
    umi_workbench_designer_gtk4_refresh_command_palette(designer);
}

GtkWidget *umi_workbench_designer_gtk4_build_command_palette(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *popover = gtk_popover_new();
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    designer->command_search = gtk_search_entry_new();
    designer->command_list = gtk_list_box_new();
    gtk_widget_set_size_request(root, 520, 360);
    g_signal_connect(
        designer->command_search, "changed",
        G_CALLBACK(command_palette_search_changed), designer);
    gtk_box_append(GTK_BOX(root), designer->command_search);
    gtk_box_append(GTK_BOX(root), designer->command_list);
    gtk_popover_set_child(GTK_POPOVER(popover), root);
    return popover;
}

void umi_workbench_designer_gtk4_refresh_command_palette(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *child;
    size_t index;
    if (designer == NULL || designer->command_list == NULL) return;
    child = gtk_widget_get_first_child(designer->command_list);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(designer->command_list), child);
        child = next;
    }
    for (index = 0U; index < designer->bundle.command_palette.count; ++index) {
        const UmiWorkbenchDesignerCommandPaletteItem *item =
            &designer->bundle.command_palette.items[index];
        GtkWidget *row;
        GtkWidget *label;
        GtkWidget *shortcut;
        if (!item->visible) continue;
        row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        label = gtk_label_new(item->label);
        shortcut = gtk_label_new(item->shortcut);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_add_css_class(shortcut, "dim-label");
        gtk_box_append(GTK_BOX(row), label);
        gtk_box_append(GTK_BOX(row), shortcut);
        gtk_list_box_append(GTK_LIST_BOX(designer->command_list), row);
    }
}
