/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workspace_profile_gtk4.c
 *
 * PURPOSE:
 *   Render the toolkit-neutral workspace-profile catalogue as a compact GTK4
 *   layout picker in the main workbench toolbar.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The picker contains no Studio-specific profile names. It reads Framework
 * snapshots, asks the Framework workbench to activate the selected profile,
 * and then uses the normal full refresh path to update every visible region.
 */

#include "gtk4_internal.h"

#include <stdio.h>

static void on_workspace_profile_clicked(GtkButton *button,
                                         gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *profile_id;
    UmiUiWorkbench *workbench;
    GtkWidget *popover;

    if (adapter == NULL || adapter->shell == NULL) return;
    profile_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-workspace-profile-id");
    if (profile_id == NULL) return;

    workbench = umi_ui_application_shell_workbench(adapter->shell);
    if (umi_ui_workbench_activate_workspace_profile(workbench, profile_id) !=
        UMI_STATUS_OK) {
        return;
    }

    popover = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_POPOVER);
    if (popover != NULL) gtk_popover_popdown(GTK_POPOVER(popover));
    (void)umi_gtk4_refresh_workbench(adapter);
}

static GtkWidget *create_profile_button(
    UmiGtk4Adapter *adapter,
    const UmiUiWorkspaceProfileSnapshot *profile)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    GtkWidget *title = gtk_label_new(profile->label);
    GtkWidget *description = gtk_label_new(profile->description);
    GtkWidget *state_icon;

    if (profile->icon_name[0] != '\0') {
        GtkWidget *icon = gtk_image_new_from_icon_name(profile->icon_name);
        gtk_image_set_pixel_size(GTK_IMAGE(icon), 17);
        gtk_box_append(GTK_BOX(row), icon);
    }

    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(description), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(description), 38);
    gtk_widget_add_css_class(description,
                             "umicom-workspace-profile-description");
    gtk_box_append(GTK_BOX(text), title);
    gtk_box_append(GTK_BOX(text), description);
    gtk_widget_set_hexpand(text, TRUE);
    gtk_box_append(GTK_BOX(row), text);

    state_icon = gtk_image_new_from_icon_name("object-select-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(state_icon), 15);
    gtk_widget_set_opacity(state_icon, profile->active ? 1.0 : 0.0);
    gtk_box_append(GTK_BOX(row), state_icon);

    gtk_button_set_child(GTK_BUTTON(button), row);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-workspace-profile-item");
    if (profile->active) gtk_widget_add_css_class(button, "active");
    gtk_widget_set_tooltip_text(button, profile->description);
    g_object_set_data_full(G_OBJECT(button),
                           "umicom-workspace-profile-id",
                           g_strdup(profile->profile_id),
                           g_free);
    g_signal_connect(button,
                     "clicked",
                     G_CALLBACK(on_workspace_profile_clicked),
                     adapter);
    return button;
}

UmiStatus umi_gtk4_refresh_workspace_profiles(
    UmiGtk4Adapter *adapter,
    UmiUiWorkbench *workbench)
{
    UmiUiWorkspaceProfileModel *model;
    GtkWidget *popover;
    GtkWidget *items_box;
    size_t index;
    size_t count;
    UmiUiWorkbenchState state;
    char active_label[UMI_UI_TEXT_CAPACITY + 8U] = "Layout";

    if (adapter == NULL || workbench == NULL ||
        adapter->workspace_profile_button == NULL ||
        adapter->workspace_profile_label == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    model = umi_ui_workbench_workspace_profiles(workbench);
    count = umi_ui_workspace_profile_model_count(model);
    umi_ui_workbench_state_init(&state);
    (void)umi_ui_workbench_state_snapshot(workbench, &state);
    gtk_widget_set_visible(adapter->workspace_profile_button, count > 0U);
    if (count == 0U) return UMI_STATUS_OK;

    popover = gtk_popover_new();
    gtk_widget_add_css_class(popover,
                             "umicom-workspace-profile-popover");
    items_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);

    for (index = 0U; index < count; ++index) {
        UmiUiWorkspaceProfileSnapshot profile;
        if (umi_ui_workspace_profile_model_at(model, index, &profile) !=
            UMI_STATUS_OK) {
            continue;
        }
        if (profile.active) {
            int modified =
                profile.sidebar_visible != state.sidebar_visible ||
                profile.auxiliary_sidebar_visible !=
                    state.auxiliary_sidebar_visible ||
                profile.bottom_panel_visible != state.bottom_panel_visible ||
                profile.sidebar_size != state.sidebar_size ||
                profile.auxiliary_sidebar_size !=
                    state.auxiliary_sidebar_size ||
                profile.bottom_panel_size != state.bottom_panel_size;
            if (modified) {
                (void)snprintf(active_label, sizeof(active_label),
                               "%s •", profile.label);
            } else {
                (void)snprintf(active_label, sizeof(active_label),
                               "%s", profile.label);
            }
        }
        gtk_box_append(GTK_BOX(items_box),
                       create_profile_button(adapter, &profile));
    }

    gtk_label_set_text(GTK_LABEL(adapter->workspace_profile_label),
                       active_label);
    gtk_popover_set_child(GTK_POPOVER(popover), items_box);
    gtk_menu_button_set_popover(
        GTK_MENU_BUTTON(adapter->workspace_profile_button), popover);
    return UMI_STATUS_OK;
}
