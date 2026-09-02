/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/quick_access_gtk4.c
 *
 * PURPOSE:
 *   Render command-palette search results and execute selected canonical Framework commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Search metadata comes from UmiCommandRegistry. No command handler is copied into GTK code.
 */

#include "gtk4_internal.h"

/* Provide the clear list operation used by this module and its client applications. */
static void clear_list(GtkWidget *list)
{
    GtkWidget *child;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || !GTK_IS_LIST_BOX(list)) return;
    child = gtk_widget_get_first_child(list);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(list), child);
        child = next;
    }
}

/*
 * Provide the gtk4 on quick access changed operation used by this module and its client
 * applications.
 */
void umi_gtk4_on_quick_access_changed(GtkSearchEntry *entry,
                                      gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    UmiUiQuickAccessResults results;
    UmiUiWorkbench *workbench;
    const char *query;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->shell == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    query = gtk_editable_get_text(GTK_EDITABLE(entry));
    clear_list(adapter->quick_access_list);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') {
        gtk_widget_set_visible(adapter->quick_access_list, FALSE);
        return;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_workbench_quick_access(workbench, query, &results) != UMI_STATUS_OK) {
        gtk_widget_set_visible(adapter->quick_access_list, FALSE);
        return;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < results.count; ++index) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        GtkWidget *icon = results.items[index].icon_name[0] != '\0'
            ? gtk_image_new_from_icon_name(results.items[index].icon_name)
            : gtk_image_new_from_icon_name("system-run-symbolic");
        GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
        GtkWidget *title = gtk_label_new(results.items[index].title);
        char detail[UMI_COMMAND_CATEGORY_CAPACITY + UMI_COMMAND_ID_CAPACITY + 8U];
        GtkWidget *meta;
        GtkWidget *description = gtk_label_new(results.items[index].description);
        GtkWidget *accelerator = gtk_label_new(results.items[index].accelerator);

        gtk_image_set_pixel_size(GTK_IMAGE(icon), 18);
        gtk_widget_set_size_request(icon, 22, 22);
        gtk_widget_add_css_class(icon, "umicom-command-icon");
        gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
        gtk_label_set_ellipsize(GTK_LABEL(title), PANGO_ELLIPSIZE_END);
        gtk_widget_add_css_class(title, "umicom-command-title");
        (void)g_snprintf(detail, sizeof(detail), "%s  •  %s",
                         results.items[index].category,
                         results.items[index].command_id);
        meta = gtk_label_new(detail);
        gtk_label_set_xalign(GTK_LABEL(meta), 0.0F);
        gtk_label_set_ellipsize(GTK_LABEL(meta), PANGO_ELLIPSIZE_END);
        gtk_widget_add_css_class(meta, "umicom-command-meta");
        gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
        gtk_label_set_ellipsize(GTK_LABEL(description), PANGO_ELLIPSIZE_END);
        gtk_widget_add_css_class(description, "umicom-command-description");
        gtk_widget_set_visible(description,
                               results.items[index].description[0] != '\0');
        gtk_label_set_xalign(GTK_LABEL(accelerator), 1.0F);
        gtk_widget_set_valign(accelerator, GTK_ALIGN_CENTER);
        gtk_widget_add_css_class(accelerator, "umicom-command-accelerator");

        gtk_widget_set_hexpand(text_box, TRUE);
        gtk_box_append(GTK_BOX(text_box), title);
        gtk_box_append(GTK_BOX(text_box), description);
        gtk_box_append(GTK_BOX(text_box), meta);
        gtk_box_append(GTK_BOX(box), icon);
        gtk_box_append(GTK_BOX(box), text_box);
        gtk_box_append(GTK_BOX(box), accelerator);
        gtk_widget_add_css_class(box, "umicom-command-result-content");
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box);
        gtk_widget_add_css_class(row, "umicom-command-result");
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (results.items[index].checkable && results.items[index].checked) {
            gtk_widget_add_css_class(row, "checked");
        }
        gtk_widget_set_sensitive(row, results.items[index].enabled != 0);
        g_object_set_data_full(G_OBJECT(row),
                               "umicom-action-id",
                               g_strdup(results.items[index].action_id),
                               g_free);
        g_object_set_data_full(G_OBJECT(row),
                               "umicom-command-id",
                               g_strdup(results.items[index].command_id),
                               g_free);
        gtk_list_box_append(GTK_LIST_BOX(adapter->quick_access_list), row);
    }
    gtk_widget_set_visible(adapter->quick_access_list, results.count > 0U);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (results.count > 0U) {
        GtkListBoxRow *first = gtk_list_box_get_row_at_index(
            GTK_LIST_BOX(adapter->quick_access_list), 0);
        gtk_list_box_select_row(GTK_LIST_BOX(adapter->quick_access_list), first);
    }
}

/*
 * Provide the gtk4 on quick access row activated operation used by this module and its
 * client applications.
 */
void umi_gtk4_on_quick_access_row_activated(GtkListBox *list_box,
                                            GtkListBoxRow *row,
                                            gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *action_id;
    const char *command_id;
    UmiUiWorkbench *workbench;
    char message[512] = "";
    UmiStatus status;
    (void)list_box;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || row == NULL || adapter->shell == NULL) return;
    action_id = (const char *)g_object_get_data(G_OBJECT(row),
                                                "umicom-action-id");
    command_id = (const char *)g_object_get_data(G_OBJECT(row),
                                                 "umicom-command-id");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_id == NULL) return;

    gtk_editable_set_text(GTK_EDITABLE(adapter->quick_access_entry), "");
    gtk_widget_set_visible(adapter->quick_access_list, FALSE);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (action_id != NULL && action_id[0] != '\0') {
        umi_gtk4_dispatch_action(adapter, action_id);
        return;
    }

    workbench = umi_ui_application_shell_workbench(adapter->shell);
    status = umi_command_registry_execute(umi_ui_workbench_commands(workbench),
                                          command_id,
                                          NULL,
                                          message,
                                          sizeof(message));
    /* Apply this branch only when its contract condition is satisfied. */
    if (message[0] == '\0') {
        (void)g_snprintf(message, sizeof(message), "%s: %s",
                         command_id, umi_status_text(status));
    }
    gtk_label_set_text(GTK_LABEL(adapter->status_label), message);
    (void)umi_gtk4_refresh_workbench(adapter);
}

/*
 * Provide the gtk4 on quick access activate operation used by this module and its client
 * applications.
 */
void umi_gtk4_on_quick_access_activate(GtkSearchEntry *entry,
                                       gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    GtkListBoxRow *row;
    (void)entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->quick_access_list == NULL) return;
    row = gtk_list_box_get_selected_row(GTK_LIST_BOX(adapter->quick_access_list));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row == NULL) {
        row = gtk_list_box_get_row_at_index(
            GTK_LIST_BOX(adapter->quick_access_list), 0);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row != NULL) {
        umi_gtk4_on_quick_access_row_activated(
            GTK_LIST_BOX(adapter->quick_access_list), row, adapter);
    }
}

/*
 * Provide the gtk4 refresh quick access request operation used by this module and its
 * client applications.
 */
void umi_gtk4_refresh_quick_access_request(UmiGtk4Adapter *adapter,
                                           UmiUiWorkbench *workbench)
{
    UmiUiContextSnapshot request;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL ||
        umi_ui_context_get(umi_ui_workbench_context(workbench),
                           UMI_UI_QUICK_ACCESS_REQUEST_CONTEXT_KEY,
                           &request) != UMI_STATUS_OK ||
        request.kind != UMI_UI_CONTEXT_INTEGER ||
        request.integer_value == adapter->quick_access_request_seen) {
        return;
    }
    adapter->quick_access_request_seen = request.integer_value;
    gtk_widget_grab_focus(adapter->quick_access_entry);
}
