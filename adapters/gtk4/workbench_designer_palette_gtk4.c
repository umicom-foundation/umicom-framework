/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_palette_gtk4.c
 *
 * PURPOSE:
 *   Render the Framework component palette and its application/category metadata
 *   for drag and keyboard insertion.
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
 * Provide the palette search changed operation used by this module and its client
 * applications.
 */
static void palette_search_changed(GtkEditable *editable, gpointer user_data)
{
    UmiWorkbenchDesignerGtk4 *designer = user_data;
    const char *query;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL) return;
    query = gtk_editable_get_text(editable);
    (void)query;
    umi_workbench_designer_gtk4_refresh_palette(designer);
}

/*
 * Provide the workbench designer gtk4 build palette operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_palette(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *heading = gtk_label_new("Components");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_widget_add_css_class(heading, "heading");
    designer->palette_search = gtk_search_entry_new();
    g_signal_connect(
        designer->palette_search, "changed",
        G_CALLBACK(palette_search_changed), designer);
    designer->palette_list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(designer->palette_list), GTK_SELECTION_SINGLE);
    gtk_box_append(GTK_BOX(root), heading);
    gtk_box_append(GTK_BOX(root), designer->palette_search);
    gtk_box_append(GTK_BOX(root), designer->palette_list);
    return root;
}

/*
 * Provide the workbench designer gtk4 refresh palette operation used by this module and
 * its client applications.
 */
void umi_workbench_designer_gtk4_refresh_palette(
    UmiWorkbenchDesignerGtk4 *designer)
{
    UmiWorkbenchDesignerSession *session;
    const UmiWorkbenchDesignerPalette *palette;
    const char *query;
    GtkWidget *child;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL || designer->palette_list == NULL) return;
    child = gtk_widget_get_first_child(designer->palette_list);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(designer->palette_list), child);
        child = next;
    }
    session = umi_workbench_designer_service_active(
        designer->config.controller->service);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    palette = umi_workbench_designer_session_palette(session);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (palette == NULL) return;
    query = gtk_editable_get_text(GTK_EDITABLE(designer->palette_search));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < palette->count; ++index) {
        const UmiWorkbenchDesignerPaletteItem *item = &palette->items[index];
        GtkWidget *row;
        GtkWidget *label;
        GtkWidget *owner;
        /* Apply this operation only while the related capability or state is available. */
        if (!item->enabled) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (query != NULL && query[0] != '\0' &&
            !g_str_match_string(query, item->label, TRUE) &&
            !g_str_match_string(query, item->category, TRUE) &&
            !g_str_match_string(query, item->keywords, TRUE)) continue;
        row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
        label = gtk_label_new(item->label);
        owner = gtk_label_new(item->owner_application_id);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(owner), 0.0F);
        gtk_widget_add_css_class(owner, "dim-label");
        gtk_widget_set_tooltip_text(row, item->description);
        gtk_box_append(GTK_BOX(row), label);
        gtk_box_append(GTK_BOX(row), owner);
        gtk_list_box_append(GTK_LIST_BOX(designer->palette_list), row);
    }
}
