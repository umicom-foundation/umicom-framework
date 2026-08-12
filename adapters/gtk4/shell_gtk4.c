/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/shell_gtk4.c
 *
 * PURPOSE:
 *   Create the GTK4 application window and reusable IDE-style workbench hierarchy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The shell now reserves stable areas for Activity Bar, view-container header, breadcrumbs and command palette while retaining the existing pane/document/status adapters.
 */

#include "gtk4_internal.h"

void umi_gtk4_clear_box(GtkWidget *box)
{
    GtkWidget *child;
    if (box == NULL || !GTK_IS_BOX(box)) return;
    child = gtk_widget_get_first_child(box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(box), child);
        child = next;
    }
}

UmiStatus umi_gtk4_build_shell(UmiGtk4Adapter *adapter)
{
    GtkWidget *middle_paned;
    GtkWidget *centre_paned;
    GtkWidget *left_cluster;
    GtkWidget *centre_box;

    if (adapter == NULL || adapter->application == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    adapter->window = GTK_WINDOW(gtk_application_window_new(adapter->application));
    adapter->root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    adapter->menu_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    adapter->toolbar_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

    /* Command palette / quick access stays visible but compact in the toolbar. */
    adapter->quick_access_entry = gtk_search_entry_new();
    gtk_search_entry_set_placeholder_text(
        GTK_SEARCH_ENTRY(adapter->quick_access_entry),
        "Command Palette — search Framework commands"
    );
    gtk_widget_set_hexpand(adapter->quick_access_entry, TRUE);
    gtk_widget_set_size_request(adapter->quick_access_entry, 280, -1);
    gtk_box_append(GTK_BOX(adapter->toolbar_box), adapter->quick_access_entry);

    /* Results are a normal list rather than a toolkit-specific data source. */
    adapter->quick_access_list = gtk_list_box_new();
    gtk_widget_set_visible(adapter->quick_access_list, FALSE);

    adapter->content_paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    middle_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    centre_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

    left_cluster = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    adapter->activity_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_size_request(adapter->activity_box, 52, -1);
    gtk_widget_add_css_class(adapter->activity_box, "toolbar");

    adapter->sidebar_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    adapter->sidebar_header = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(adapter->sidebar_header), 0.0F);
    gtk_widget_add_css_class(adapter->sidebar_header, "heading");
    adapter->left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_size_request(adapter->sidebar_box, 280, -1);
    gtk_box_append(GTK_BOX(adapter->sidebar_box), adapter->sidebar_header);
    gtk_box_append(GTK_BOX(adapter->sidebar_box), adapter->left_box);
    gtk_widget_set_vexpand(adapter->left_box, TRUE);

    gtk_box_append(GTK_BOX(left_cluster), adapter->activity_box);
    gtk_box_append(GTK_BOX(left_cluster), adapter->sidebar_box);

    centre_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    adapter->breadcrumb_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_add_css_class(adapter->breadcrumb_box, "toolbar");
    adapter->document_notebook = gtk_notebook_new();
    gtk_widget_set_hexpand(adapter->document_notebook, TRUE);
    gtk_widget_set_vexpand(adapter->document_notebook, TRUE);
    gtk_box_append(GTK_BOX(centre_box), adapter->breadcrumb_box);
    gtk_box_append(GTK_BOX(centre_box), adapter->document_notebook);

    adapter->right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    adapter->bottom_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    adapter->notification_label = gtk_label_new("");
    adapter->status_label = gtk_label_new("");

    gtk_paned_set_start_child(GTK_PANED(centre_paned), centre_box);
    gtk_paned_set_end_child(GTK_PANED(centre_paned), adapter->right_box);
    gtk_paned_set_position(GTK_PANED(centre_paned), 900);

    gtk_paned_set_start_child(GTK_PANED(middle_paned), left_cluster);
    gtk_paned_set_end_child(GTK_PANED(middle_paned), centre_paned);
    gtk_paned_set_position(GTK_PANED(middle_paned), 340);

    gtk_paned_set_start_child(GTK_PANED(adapter->content_paned), middle_paned);
    gtk_paned_set_end_child(GTK_PANED(adapter->content_paned), adapter->bottom_box);
    gtk_paned_set_position(GTK_PANED(adapter->content_paned), 650);

    gtk_box_append(GTK_BOX(adapter->root_box), adapter->menu_bar);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->toolbar_box);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->quick_access_list);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->notification_label);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->content_paned);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->status_label);
    gtk_window_set_child(adapter->window, adapter->root_box);

    g_signal_connect(adapter->quick_access_entry,
                     "search-changed",
                     G_CALLBACK(umi_gtk4_on_quick_access_changed),
                     adapter);
    g_signal_connect(adapter->quick_access_list,
                     "row-activated",
                     G_CALLBACK(umi_gtk4_on_quick_access_row_activated),
                     adapter);

    /*
     * Install one window-level key controller. It delegates all shortcut policy
     * to Framework's context-sensitive keybinding registry.
     */
    return umi_gtk4_install_keybindings(adapter);
}
