/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/activity_bar_gtk4.c
 *
 * PURPOSE:
 *   Render the Framework Activity Bar and route activation back to the workbench model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The button is only a GTK presentation of UmiUiActivitySnapshot. The active activity remains authoritative in Framework.
 */

#include "gtk4_internal.h"

static void on_activity_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *activity_id;
    UmiUiWorkbench *workbench;
    if (adapter == NULL || adapter->shell == NULL) return;

    activity_id = (const char *)g_object_get_data(G_OBJECT(button),
                                                  "umicom-activity-id");
    if (activity_id == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    if (umi_ui_workbench_activate_activity(workbench, activity_id) == UMI_STATUS_OK) {
        (void)umi_gtk4_refresh_workbench(adapter);
    }
}

UmiStatus umi_gtk4_refresh_activity_bar(UmiGtk4Adapter *adapter,
                                       UmiUiWorkbench *workbench)
{
    UmiUiActivityModel *model;
    size_t index;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_gtk4_clear_box(adapter->activity_box);
    model = umi_ui_workbench_activities(workbench);

    for (index = 0U; index < umi_ui_activity_model_count(model); ++index) {
        UmiUiActivitySnapshot activity;
        GtkWidget *button;
        if (umi_ui_activity_model_at(model, index, &activity) != UMI_STATUS_OK ||
            !activity.visible) {
            continue;
        }

        button = gtk_button_new();
        if (activity.icon_name[0] != '\0') {
            gtk_button_set_icon_name(GTK_BUTTON(button), activity.icon_name);
        } else {
            gtk_button_set_label(GTK_BUTTON(button), activity.label);
        }
        gtk_widget_set_tooltip_text(button, activity.label);
        gtk_widget_set_sensitive(button, activity.enabled != 0);
        gtk_widget_add_css_class(button, "flat");
        gtk_widget_add_css_class(button, "umicom-activity-button");
        gtk_widget_set_size_request(button, 40, 40);
        if (activity.active) {
            gtk_widget_add_css_class(button, "active");
        }

        g_object_set_data_full(G_OBJECT(button),
                               "umicom-activity-id",
                               g_strdup(activity.activity_id),
                               g_free);
        g_signal_connect(button,
                         "clicked",
                         G_CALLBACK(on_activity_clicked),
                         adapter);
        gtk_box_append(GTK_BOX(adapter->activity_box), button);
    }
    return UMI_STATUS_OK;
}
