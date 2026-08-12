/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/quick_access_gtk4.c
 *
 * PURPOSE:
 *   Render command-palette search results and execute selected canonical Framework commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Search metadata comes from UmiCommandRegistry. No command handler is copied into GTK code.
 */

#include "gtk4_internal.h"

static void clear_list(GtkWidget *list)
{
    GtkWidget *child;
    if (list == NULL || !GTK_IS_LIST_BOX(list)) return;
    child = gtk_widget_get_first_child(list);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(list), child);
        child = next;
    }
}

void umi_gtk4_on_quick_access_changed(GtkSearchEntry *entry,
                                      gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    UmiUiQuickAccessResults results;
    UmiUiWorkbench *workbench;
    const char *query;
    size_t index;

    if (adapter == NULL || adapter->shell == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    query = gtk_editable_get_text(GTK_EDITABLE(entry));
    clear_list(adapter->quick_access_list);

    if (query == NULL || query[0] == '\0') {
        gtk_widget_set_visible(adapter->quick_access_list, FALSE);
        return;
    }

    if (umi_ui_workbench_quick_access(workbench, query, &results) != UMI_STATUS_OK) {
        gtk_widget_set_visible(adapter->quick_access_list, FALSE);
        return;
    }

    for (index = 0U; index < results.count; ++index) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
        GtkWidget *title = gtk_label_new(results.items[index].title);
        char detail[UMI_COMMAND_CATEGORY_CAPACITY + UMI_COMMAND_ID_CAPACITY + 8U];
        GtkWidget *meta;

        gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
        (void)g_snprintf(detail, sizeof(detail), "%s  •  %s",
                         results.items[index].category,
                         results.items[index].command_id);
        meta = gtk_label_new(detail);
        gtk_label_set_xalign(GTK_LABEL(meta), 0.0F);
        gtk_widget_add_css_class(meta, "dim-label");

        gtk_box_append(GTK_BOX(box), title);
        gtk_box_append(GTK_BOX(box), meta);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box);
        gtk_widget_set_sensitive(row, results.items[index].enabled != 0);
        g_object_set_data_full(G_OBJECT(row),
                               "umicom-command-id",
                               g_strdup(results.items[index].command_id),
                               g_free);
        gtk_list_box_append(GTK_LIST_BOX(adapter->quick_access_list), row);
    }
    gtk_widget_set_visible(adapter->quick_access_list, results.count > 0U);
}

void umi_gtk4_on_quick_access_row_activated(GtkListBox *list_box,
                                            GtkListBoxRow *row,
                                            gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *command_id;
    UmiUiWorkbench *workbench;
    char message[512] = "";
    UmiStatus status;
    (void)list_box;

    if (adapter == NULL || row == NULL || adapter->shell == NULL) return;
    command_id = (const char *)g_object_get_data(G_OBJECT(row),
                                                 "umicom-command-id");
    if (command_id == NULL) return;

    workbench = umi_ui_application_shell_workbench(adapter->shell);
    status = umi_command_registry_execute(umi_ui_workbench_commands(workbench),
                                          command_id,
                                          NULL,
                                          message,
                                          sizeof(message));
    if (message[0] == '\0') {
        (void)g_snprintf(message, sizeof(message), "%s: %s",
                         command_id, umi_status_text(status));
    }
    gtk_label_set_text(GTK_LABEL(adapter->status_label), message);
    gtk_editable_set_text(GTK_EDITABLE(adapter->quick_access_entry), "");
    gtk_widget_set_visible(adapter->quick_access_list, FALSE);
    (void)umi_gtk4_refresh_workbench(adapter);
}
