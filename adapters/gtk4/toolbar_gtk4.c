/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/toolbar_gtk4.c
 *
 * PURPOSE:
 *   Render toolbar action contributions and route button activation through the shared
 *   Framework workbench command path.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

static void on_button_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *action_id = (const char *)g_object_get_data(G_OBJECT(button), "umicom-action-id");
    char message[512];
    if (adapter != NULL && adapter->shell != NULL && action_id != NULL) {
        UmiUiWorkbench *workbench = umi_ui_application_shell_workbench(adapter->shell);
        if (umi_ui_workbench_execute_action(workbench, action_id, "", message, sizeof(message)) == UMI_STATUS_OK) {
            gtk_label_set_text(GTK_LABEL(adapter->status_label), message);
        }
    }
}

UmiStatus umi_gtk4_refresh_toolbar(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench)
{
    UmiUiToolbarModel *toolbar;
    UmiUiActionModel *actions;
    size_t index;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_gtk4_clear_box(adapter->toolbar_box);
    /* Quick access is permanent workbench chrome, not a product contribution. */
    gtk_box_append(GTK_BOX(adapter->toolbar_box), adapter->quick_access_entry);
    toolbar = umi_ui_workbench_toolbars(workbench);
    actions = umi_ui_workbench_actions(workbench);
    for (index = 0U; index < umi_ui_toolbar_model_count(toolbar); ++index) {
        UmiUiToolbarSnapshot item;
        if (umi_ui_toolbar_model_at(toolbar, index, &item) == UMI_STATUS_OK) {
            if (item.separator) {
                gtk_box_append(GTK_BOX(adapter->toolbar_box), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
            } else {
                UmiUiActionSnapshot action;
                if (umi_ui_action_model_find(actions, item.action_id, &action) == UMI_STATUS_OK && action.visible) {
                    GtkWidget *button = gtk_button_new_with_label(action.label);
                    gtk_widget_set_sensitive(button, action.enabled);
                    gtk_widget_set_tooltip_text(button, action.tooltip);
                    g_object_set_data_full(G_OBJECT(button), "umicom-action-id", g_strdup(action.action_id), g_free);
                    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), adapter);
                    gtk_box_append(GTK_BOX(adapter->toolbar_box), button);
                }
            }
        }
    }
    return UMI_STATUS_OK;
}
