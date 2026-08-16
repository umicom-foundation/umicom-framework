/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desktop_shell_gtk4.c
 *
 * PURPOSE:
 *   Render Framework-owned bottom layout tabs and desktop status without
 *   placing application or geometry policy inside the GTK4 adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
    char monitor_text[64U];
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
    for (index = 0U; index < snapshot.tab_count; ++index) {
        UmiDesktopShellTab tab;
        status = umi_desktop_shell_model_tab_at(
            adapter->desktop_shell, index, &tab);
        if (status != UMI_STATUS_OK) return status;
        gtk_box_append(GTK_BOX(adapter->desktop_layout_tabs_box),
                       layout_tab_button(adapter, &tab));
    }
    (void)snprintf(monitor_text, sizeof(monitor_text), "%zu monitor%s",
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
