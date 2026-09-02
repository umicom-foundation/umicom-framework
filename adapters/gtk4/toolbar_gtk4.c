/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/toolbar_gtk4.c
 *
 * PURPOSE:
 *   Render toolbar action contributions and route button activation through the shared
 *   Framework workbench command path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

/* Provide the on button clicked operation used by this module and its client applications. */
static void on_button_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *action_id = (const char *)g_object_get_data(G_OBJECT(button), "umicom-action-id");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter != NULL && adapter->shell != NULL && action_id != NULL) {
        umi_gtk4_dispatch_action(adapter, action_id);
    }
}

/*
 * Provide the create action button operation used by this module and its client
 * applications.
 */
static GtkWidget *create_action_button(const UmiUiActionSnapshot *action)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *label = gtk_label_new(action->label);

    /* Apply this branch only when its contract condition is satisfied. */
    if (action->icon_name[0] != '\0') {
        GtkWidget *icon = gtk_image_new_from_icon_name(action->icon_name);
        gtk_image_set_pixel_size(GTK_IMAGE(icon), 15);
        gtk_box_append(GTK_BOX(content), icon);
    }
    gtk_box_append(GTK_BOX(content), label);
    gtk_button_set_child(GTK_BUTTON(button), content);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-toolbar-button");
    return button;
}

/*
 * Provide the gtk4 refresh toolbar operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_toolbar(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench)
{
    UmiUiToolbarModel *toolbar;
    UmiUiActionModel *actions;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Only contribution widgets are rebuilt. Permanent project and command
     * search widgets remain owned by the shell and therefore never pass
     * through an unsafe remove/re-append lifecycle.
     */
    umi_gtk4_clear_box(adapter->toolbar_actions_box);
    toolbar = umi_ui_workbench_toolbars(workbench);
    actions = umi_ui_workbench_actions(workbench);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ui_toolbar_model_count(toolbar); ++index) {
        UmiUiToolbarSnapshot item;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_ui_toolbar_model_at(toolbar, index, &item) == UMI_STATUS_OK) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (item.separator) {
                GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
                gtk_widget_add_css_class(separator, "umicom-toolbar-separator");
                gtk_box_append(GTK_BOX(adapter->toolbar_actions_box), separator);
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                UmiUiActionSnapshot action;
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (umi_ui_action_model_find(actions, item.action_id, &action) == UMI_STATUS_OK && action.visible) {
                    GtkWidget *button = create_action_button(&action);
                    /* UAT activates toolbar actions through their canonical action IDs. */
                    (void)umi_gtk4_automation_tag_widget(
                        button,
                        action.action_id);
                    gtk_widget_set_sensitive(button, action.enabled);
                    gtk_widget_set_tooltip_text(button, action.tooltip);
                    g_object_set_data_full(G_OBJECT(button), "umicom-action-id", g_strdup(action.action_id), g_free);
                    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), adapter);
                    gtk_box_append(GTK_BOX(adapter->toolbar_actions_box), button);
                }
            }
        }
    }
    return UMI_STATUS_OK;
}
