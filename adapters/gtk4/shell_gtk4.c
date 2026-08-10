/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/shell_gtk4.c
 *
 * PURPOSE:
 *   Create the GTK4 application window and stable workbench container hierarchy used
 *   by the reference adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
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
    if (adapter == NULL || adapter->application == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    adapter->window = GTK_WINDOW(gtk_application_window_new(adapter->application));
    adapter->root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    adapter->menu_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    adapter->toolbar_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    adapter->content_paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    middle_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    centre_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    adapter->left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    adapter->document_notebook = gtk_notebook_new();
    adapter->right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    adapter->bottom_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    adapter->notification_label = gtk_label_new("");
    adapter->status_label = gtk_label_new("");
    gtk_widget_set_hexpand(adapter->document_notebook, TRUE);
    gtk_widget_set_vexpand(adapter->document_notebook, TRUE);
    gtk_paned_set_start_child(GTK_PANED(centre_paned), adapter->document_notebook);
    gtk_paned_set_end_child(GTK_PANED(centre_paned), adapter->right_box);
    gtk_paned_set_start_child(GTK_PANED(middle_paned), adapter->left_box);
    gtk_paned_set_end_child(GTK_PANED(middle_paned), centre_paned);
    gtk_paned_set_start_child(GTK_PANED(adapter->content_paned), middle_paned);
    gtk_paned_set_end_child(GTK_PANED(adapter->content_paned), adapter->bottom_box);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->menu_bar);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->toolbar_box);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->notification_label);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->content_paned);
    gtk_box_append(GTK_BOX(adapter->root_box), adapter->status_label);
    gtk_window_set_child(adapter->window, adapter->root_box);
    return UMI_STATUS_OK;
}
