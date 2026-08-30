/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desktop_shell_gtk4.c
 *
 * PURPOSE:
 *   Render the Framework-owned application taskbar, bottom layout tabs and
 *   desktop status without placing application or geometry policy inside GTK4.
 *
 * DESIGN NOTE:
 *   The taskbar displays validated Framework application metadata. Clicking an
 *   item updates the toolkit-neutral active-application model and selects its
 *   default layout when available. Process launch/supervision remains a later
 *   Framework launcher responsibility rather than ad-hoc GTK process code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

#include <stdio.h>

static void activate_matching_workbench_profile(
    UmiGtk4Adapter *adapter,
    const char *layout_id)
{
    UmiUiWorkbench *workbench;
    UmiUiWorkspaceProfileSnapshot profile;
    if (adapter == NULL || adapter->shell == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    if (umi_ui_workspace_profile_model_find(
            umi_ui_workbench_workspace_profiles(workbench),
            layout_id, &profile) == UMI_STATUS_OK) {
        (void)umi_ui_workbench_activate_workspace_profile(
            workbench, layout_id);
    }
}

static void on_layout_tab_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *layout_id;
    if (adapter == NULL || adapter->desktop_shell == NULL) return;
    layout_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-desktop-layout-id");
    if (layout_id == NULL) return;
    if (umi_desktop_shell_model_activate_layout(
            adapter->desktop_shell, layout_id) == UMI_STATUS_OK) {
        activate_matching_workbench_profile(adapter, layout_id);
        (void)umi_gtk4_refresh_workbench(adapter);
    }
}

static void on_application_button_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    UmiDesktopTaskbarItem item;
    const char *application_id;
    if (adapter == NULL || adapter->desktop_shell == NULL) return;
    application_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-desktop-application-id");
    if (application_id == NULL) return;
    if (umi_desktop_shell_model_activate_application(
            adapter->desktop_shell, application_id) != UMI_STATUS_OK)
        return;
    if (umi_desktop_shell_model_find_application(
            adapter->desktop_shell, application_id, &item) == UMI_STATUS_OK &&
        item.default_layout_id[0] != '\0' &&
        umi_desktop_shell_model_activate_layout(
            adapter->desktop_shell, item.default_layout_id) == UMI_STATUS_OK) {
        activate_matching_workbench_profile(adapter, item.default_layout_id);
    }
    (void)umi_gtk4_refresh_desktop_shell(adapter);
    (void)umi_gtk4_refresh_workbench(adapter);
}

static GtkWidget *application_taskbar_button(
    UmiGtk4Adapter *adapter,
    const UmiDesktopTaskbarItem *item)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *icon = gtk_image_new_from_icon_name(
        item->icon_name[0] != '\0'
            ? item->icon_name
            : "application-x-executable-symbolic");
    GtkWidget *state = gtk_label_new(
        item->attention ? "!" : (item->running ? "•" : ""));
    char tooltip[UMI_DESKTOP_DESCRIPTION_CAPACITY];
    (void)snprintf(tooltip, sizeof(tooltip), "%s · %s%s%s",
                   item->display_name,
                   umi_desktop_application_state_text(item->state),
                   item->pinned ? " · pinned" : "",
                   item->active ? " · active" : "");
    gtk_image_set_pixel_size(GTK_IMAGE(icon), 20);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-desktop-application-button");
    if (item->active) gtk_widget_add_css_class(button, "active");
    if (item->running) gtk_widget_add_css_class(button, "running");
    if (item->pinned) gtk_widget_add_css_class(button, "pinned");
    if (item->attention)
        gtk_widget_add_css_class(button, "requires-attention");
    gtk_widget_add_css_class(state, "umicom-desktop-application-state");
    gtk_box_append(GTK_BOX(box), icon);
    gtk_box_append(GTK_BOX(box), state);
    gtk_button_set_child(GTK_BUTTON(button), box);
    gtk_widget_set_tooltip_text(button, tooltip);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(button), GTK_ACCESSIBLE_PROPERTY_LABEL,
        item->display_name, -1);
    g_object_set_data_full(G_OBJECT(button),
                           "umicom-desktop-application-id",
                           g_strdup(item->application_id), g_free);
    g_signal_connect(button, "clicked",
                     G_CALLBACK(on_application_button_clicked), adapter);
    return button;
}

static GtkWidget *layout_tab_button(
    UmiGtk4Adapter *adapter,
    const UmiDesktopShellTab *tab)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *label = gtk_label_new(tab->label);
    GtkWidget *state = gtk_label_new(tab->dirty ? "•" : "");
    char tooltip[UMI_DESKTOP_DESCRIPTION_CAPACITY];
    (void)snprintf(tooltip, sizeof(tooltip), "%s layout%s%s",
                   tab->category,
                   tab->pinned ? " · Framework preset" : " · Custom",
                   tab->dirty ? " · Unsaved changes" : "");
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-desktop-layout-tab");
    if (tab->active) gtk_widget_add_css_class(button, "active");
    if (tab->dirty) gtk_widget_add_css_class(button, "dirty");
    gtk_widget_add_css_class(state, "umicom-desktop-layout-state");
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), state);
    gtk_button_set_child(GTK_BUTTON(button), box);
    gtk_widget_set_tooltip_text(button, tooltip);
    g_object_set_data_full(G_OBJECT(button),
                           "umicom-desktop-layout-id",
                           g_strdup(tab->layout_id), g_free);
    g_signal_connect(button, "clicked",
                     G_CALLBACK(on_layout_tab_clicked), adapter);
    return button;
}

UmiStatus umi_gtk4_refresh_desktop_shell(UmiGtk4Adapter *adapter)
{
    UmiDesktopShellSnapshot snapshot;
    GtkWidget *popover;
    char monitor_text[128U];
    size_t index;
    UmiStatus status;
    if (adapter == NULL || adapter->desktop_layout_bar == NULL ||
        adapter->desktop_layout_tabs_box == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->desktop_shell == NULL) {
        gtk_widget_set_visible(adapter->desktop_layout_bar, FALSE);
        return UMI_STATUS_OK;
    }
    status = umi_desktop_shell_model_snapshot(
        adapter->desktop_shell, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    umi_gtk4_clear_box(adapter->desktop_layout_tabs_box);

    /* Application icons and layout tabs deliberately share the existing bottom
     * strip. This mirrors the supplied Umicom Desk mock-up while keeping the
     * two concepts distinct in the Framework model. */
    for (index = 0U; index < snapshot.application_count; ++index) {
        UmiDesktopTaskbarItem item;
        status = umi_desktop_shell_model_application_at(
            adapter->desktop_shell, index, &item);
        if (status != UMI_STATUS_OK) return status;
        gtk_box_append(GTK_BOX(adapter->desktop_layout_tabs_box),
                       application_taskbar_button(adapter, &item));
    }
    if (snapshot.application_count > 0U && snapshot.tab_count > 0U) {
        GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
        gtk_widget_add_css_class(separator, "umicom-desktop-strip-separator");
        gtk_box_append(GTK_BOX(adapter->desktop_layout_tabs_box), separator);
    }
    for (index = 0U; index < snapshot.tab_count; ++index) {
        UmiDesktopShellTab tab;
        status = umi_desktop_shell_model_tab_at(
            adapter->desktop_shell, index, &tab);
        if (status != UMI_STATUS_OK) return status;
        gtk_box_append(GTK_BOX(adapter->desktop_layout_tabs_box),
                       layout_tab_button(adapter, &tab));
    }
    (void)snprintf(monitor_text, sizeof(monitor_text),
                   "%zu app%s · %zu running · %zu monitor%s",
                   snapshot.application_count,
                   snapshot.application_count == 1U ? "" : "s",
                   snapshot.running_application_count,
                   snapshot.monitor_count,
                   snapshot.monitor_count == 1U ? "" : "s");
    gtk_label_set_text(GTK_LABEL(adapter->desktop_monitor_label), monitor_text);
    popover = umi_gtk4_desktop_designer_popover(adapter);
    if (popover == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    gtk_menu_button_set_popover(
        GTK_MENU_BUTTON(adapter->desktop_designer_button), popover);
    gtk_widget_set_visible(adapter->desktop_layout_bar, TRUE);
    return UMI_STATUS_OK;
}
