/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_tree_gtk4.c
 *
 * PURPOSE:
 *   Render the semantic layout hierarchy as an accessible GTK4 list without
 *   relying on deprecated tree widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


GtkWidget *umi_workbench_designer_gtk4_build_tree(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *heading = gtk_label_new("Layout Tree");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_widget_add_css_class(heading, "heading");
    designer->tree_list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(designer->tree_list), GTK_SELECTION_SINGLE);
    gtk_box_append(GTK_BOX(root), heading);
    gtk_box_append(GTK_BOX(root), designer->tree_list);
    return root;
}

void umi_workbench_designer_gtk4_refresh_tree(
    UmiWorkbenchDesignerGtk4 *designer)
{
    UmiWorkbenchDesignerSession *session;
    const UmiWorkbenchDesignerTree *tree;
    GtkWidget *child;
    size_t index;
    if (designer == NULL || designer->tree_list == NULL) return;
    child = gtk_widget_get_first_child(designer->tree_list);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(designer->tree_list), child);
        child = next;
    }
    session = umi_workbench_designer_service_active(
        designer->config.controller->service);
    if (session == NULL) return;
    tree = umi_workbench_designer_session_tree(session);
    if (tree == NULL) return;
    for (index = 0U; index < tree->row_count; ++index) {
        const UmiWorkbenchDesignerTreeRow *row = &tree->rows[index];
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
        GtkWidget *indent = gtk_label_new("");
        GtkWidget *label = gtk_label_new(row->title);
        gtk_widget_set_size_request(indent, (int)(row->depth * 14U), -1);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        if (row->selected) gtk_widget_add_css_class(box, "selected");
        if (!row->visible) gtk_widget_add_css_class(label, "dim-label");
        gtk_box_append(GTK_BOX(box), indent);
        gtk_box_append(GTK_BOX(box), label);
        gtk_list_box_append(GTK_LIST_BOX(designer->tree_list), box);
    }
}
