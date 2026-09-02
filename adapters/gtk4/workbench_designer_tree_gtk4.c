/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_tree_gtk4.c
 *
 * PURPOSE:
 *   Render the semantic layout hierarchy as an accessible GTK4 list without
 *   relying on deprecated tree widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


/*
 * Provide the workbench designer gtk4 build tree operation used by this module and its
 * client applications.
 */
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

/*
 * Provide the workbench designer gtk4 refresh tree operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_gtk4_refresh_tree(
    UmiWorkbenchDesignerGtk4 *designer)
{
    UmiWorkbenchDesignerSession *session;
    const UmiWorkbenchDesignerTree *tree;
    GtkWidget *child;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || designer->tree_list == NULL) return;
    child = gtk_widget_get_first_child(designer->tree_list);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(designer->tree_list), child);
        child = next;
    }
    session = umi_workbench_designer_service_active(
        designer->config.controller->service);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    tree = umi_workbench_designer_session_tree(session);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < tree->row_count; ++index) {
        const UmiWorkbenchDesignerTreeRow *row = &tree->rows[index];
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
        GtkWidget *indent = gtk_label_new("");
        GtkWidget *label = gtk_label_new(row->title);
        gtk_widget_set_size_request(indent, (int)(row->depth * 14U), -1);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        /* Apply this branch only when its contract condition is satisfied. */
        if (row->selected) gtk_widget_add_css_class(box, "selected");
        /* Apply this operation only while the related capability or state is available. */
        if (!row->visible) gtk_widget_add_css_class(label, "dim-label");
        gtk_box_append(GTK_BOX(box), indent);
        gtk_box_append(GTK_BOX(box), label);
        gtk_list_box_append(GTK_LIST_BOX(designer->tree_list), box);
    }
}
