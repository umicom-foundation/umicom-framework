/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desk_gtk4.c
 *
 * PURPOSE:
 *   Render the Framework-owned Umicom Desk outer shell: global command area,
 *   active workbench canvas, application taskbar and semantic layout strip.
 *
 * DESIGN:
 *   This adapter translates copied Framework snapshots into GTK4 widgets. It
 *   does not scan repositories, launch programs directly, own application
 *   policy or serialise GTK widget trees as layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/desk.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiGtk4Desk {
    GtkApplication *application;
    GtkWidget *window;
    GtkWidget *application_strip;
    GtkWidget *layout_strip;
    GtkWidget *workbench_title;
    GtkWidget *workbench_description;
    GtkWidget *status_label;
    UmiDeskRuntime *runtime;
};

static void clear_box(GtkWidget *box)
{
    GtkWidget *child;
    if (box == NULL) return;
    child = gtk_widget_get_first_child(box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(box), child);
        child = next;
    }
}

static void set_status(UmiGtk4Desk *desk, const char *message)
{
    if (desk == NULL || desk->status_label == NULL) return;
    gtk_label_set_text(GTK_LABEL(desk->status_label),
                       message != NULL ? message : "");
}

static GtkWidget *make_global_button(const char *label)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-desk-global-button");
    return button;
}

static void on_application_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Desk *desk = (UmiGtk4Desk *)user_data;
    const char *application_id;
    UmiStatus status;
    char message[256U];
    if (desk == NULL) return;
    application_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-application-id");
    if (application_id == NULL) return;
    status = umi_desk_runtime_request_application(
        desk->runtime, application_id,
        UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE);
    (void)snprintf(message, sizeof(message), "%s: %s",
                   application_id, umi_status_text(status));
    set_status(desk, message);
    (void)umi_gtk4_desk_refresh(desk);
}

static void on_layout_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Desk *desk = (UmiGtk4Desk *)user_data;
    const char *layout_id;
    UmiStatus status;
    char message[256U];
    if (desk == NULL) return;
    layout_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-layout-id");
    if (layout_id == NULL) return;
    status = umi_desk_runtime_activate_layout(desk->runtime, layout_id);
    (void)snprintf(message, sizeof(message), "Layout %s: %s",
                   layout_id, umi_status_text(status));
    set_status(desk, message);
    (void)umi_gtk4_desk_refresh(desk);
}

static GtkWidget *make_application_button(
    UmiGtk4Desk *desk,
    const UmiDesktopApplicationStripItem *item)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *icon = gtk_image_new_from_icon_name(
        item->icon_name[0] != '\0'
            ? item->icon_name
            : "application-x-executable-symbolic");
    GtkWidget *label = gtk_label_new(item->display_name);
    GtkWidget *state = gtk_label_new(
        item->attention ? "!" : (item->running ? "•" : ""));
    char tooltip[512U];

    gtk_image_set_pixel_size(GTK_IMAGE(icon), 20);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(label, FALSE);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-desk-application-button");
    gtk_widget_add_css_class(state, "umicom-desk-application-state");
    if (item->active) gtk_widget_add_css_class(button, "active");
    if (item->running) gtk_widget_add_css_class(button, "running");
    if (item->pinned) gtk_widget_add_css_class(button, "pinned");
    if (item->attention) {
        gtk_widget_add_css_class(button, "requires-attention");
    }

    gtk_box_append(GTK_BOX(content), icon);
    gtk_box_append(GTK_BOX(content), label);
    gtk_box_append(GTK_BOX(content), state);
    gtk_button_set_child(GTK_BUTTON(button), content);

    (void)snprintf(tooltip, sizeof(tooltip),
                   "%s · %s%s%s",
                   item->display_name,
                   umi_application_runtime_state_text(item->state),
                   item->pinned ? " · pinned" : "",
                   item->active ? " · active" : "");
    gtk_widget_set_tooltip_text(button, tooltip);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(button),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        item->display_name,
        -1);
    g_object_set_data_full(
        G_OBJECT(button),
        "umicom-application-id",
        g_strdup(item->application_id),
        g_free);
    g_signal_connect(
        button, "clicked", G_CALLBACK(on_application_clicked), desk);
    return button;
}

static GtkWidget *make_layout_button(
    UmiGtk4Desk *desk,
    const UmiDesktopShellTab *tab)
{
    GtkWidget *button = gtk_button_new_with_label(tab->label);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-desk-layout-button");
    if (tab->active) gtk_widget_add_css_class(button, "active");
    if (tab->dirty) gtk_widget_add_css_class(button, "dirty");
    if (tab->pinned) gtk_widget_add_css_class(button, "pinned");
    g_object_set_data_full(
        G_OBJECT(button),
        "umicom-layout-id",
        g_strdup(tab->layout_id),
        g_free);
    g_signal_connect(
        button, "clicked", G_CALLBACK(on_layout_clicked), desk);
    return button;
}

static GtkWidget *build_top_bar(void)
{
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *brand = gtk_label_new("Umicom Desk");
    GtkWidget *search = gtk_search_entry_new();
    gtk_widget_add_css_class(bar, "umicom-desk-global-bar");
    gtk_widget_add_css_class(brand, "title-2");
    gtk_widget_set_margin_start(bar, 8);
    gtk_widget_set_margin_end(bar, 8);
    gtk_widget_set_margin_top(bar, 5);
    gtk_widget_set_margin_bottom(bar, 5);
    gtk_box_append(GTK_BOX(bar), brand);
    gtk_box_append(GTK_BOX(bar), make_global_button("File"));
    gtk_box_append(GTK_BOX(bar), make_global_button("Account"));
    gtk_box_append(GTK_BOX(bar), make_global_button("Help"));
    gtk_widget_set_hexpand(search, TRUE);
    gtk_widget_set_halign(search, GTK_ALIGN_END);
    gtk_widget_set_size_request(search, 320, -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(search),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Search applications, commands, layouts and settings",
        -1);
    gtk_box_append(GTK_BOX(bar), search);
    return bar;
}

static GtkWidget *build_workbench_placeholder(UmiGtk4Desk *desk)
{
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *title = gtk_label_new("Umicom Desk");
    GtkWidget *description = gtk_label_new(
        "Select an application from the taskbar. "
        "Framework-owned panels and layouts will be hosted here.");
    gtk_widget_add_css_class(frame, "umicom-desk-workbench");
    gtk_widget_add_css_class(title, "title-1");
    gtk_label_set_wrap(GTK_LABEL(description), TRUE);
    gtk_label_set_justify(GTK_LABEL(description), GTK_JUSTIFY_CENTER);
    gtk_widget_set_valign(content, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(content, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(content, TRUE);
    gtk_widget_set_vexpand(content, TRUE);
    gtk_box_append(GTK_BOX(content), title);
    gtk_box_append(GTK_BOX(content), description);
    gtk_frame_set_child(GTK_FRAME(frame), content);
    desk->workbench_title = title;
    desk->workbench_description = description;
    return frame;
}

static GtkWidget *build_bottom_area(UmiGtk4Desk *desk)
{
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *applications = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *layouts = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *status = gtk_label_new("Umicom Desk ready");
    gtk_widget_add_css_class(outer, "umicom-desk-bottom");
    gtk_widget_add_css_class(applications, "umicom-desk-application-strip");
    gtk_widget_add_css_class(layouts, "umicom-desk-layout-strip");
    gtk_widget_add_css_class(status, "dim-label");
    gtk_widget_set_margin_start(applications, 5);
    gtk_widget_set_margin_end(applications, 5);
    gtk_widget_set_margin_top(applications, 4);
    gtk_widget_set_margin_bottom(applications, 4);
    gtk_widget_set_margin_start(layouts, 5);
    gtk_widget_set_margin_end(layouts, 5);
    gtk_widget_set_margin_top(layouts, 2);
    gtk_widget_set_margin_bottom(layouts, 4);
    gtk_widget_set_halign(status, GTK_ALIGN_END);
    gtk_widget_set_margin_end(status, 8);
    gtk_box_append(GTK_BOX(outer), applications);
    gtk_box_append(GTK_BOX(outer), gtk_separator_new(
        GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(outer), layouts);
    gtk_box_append(GTK_BOX(outer), status);
    desk->application_strip = applications;
    desk->layout_strip = layouts;
    desk->status_label = status;
    return outer;
}

UmiStatus umi_gtk4_desk_create(
    void *native_gtk_application,
    UmiDeskRuntime *runtime,
    UmiGtk4Desk **out_desk)
{
    UmiGtk4Desk *desk;
    GtkWidget *root;
    if (native_gtk_application == NULL || runtime == NULL ||
        out_desk == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_desk = NULL;
    desk = (UmiGtk4Desk *)calloc(1U, sizeof(*desk));
    if (desk == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    desk->application = GTK_APPLICATION(native_gtk_application);
    desk->runtime = runtime;
    desk->window = gtk_application_window_new(desk->application);
    gtk_window_set_title(GTK_WINDOW(desk->window), "Umicom Desk");
    gtk_window_set_default_size(GTK_WINDOW(desk->window), 1480, 900);

    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(root, "umicom-desk-root");
    gtk_box_append(GTK_BOX(root), build_top_bar());
    gtk_box_append(GTK_BOX(root), gtk_separator_new(
        GTK_ORIENTATION_HORIZONTAL));
    {
        GtkWidget *workbench = build_workbench_placeholder(desk);
        gtk_widget_set_hexpand(workbench, TRUE);
        gtk_widget_set_vexpand(workbench, TRUE);
        gtk_box_append(GTK_BOX(root), workbench);
    }
    gtk_box_append(GTK_BOX(root), gtk_separator_new(
        GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(root), build_bottom_area(desk));
    gtk_window_set_child(GTK_WINDOW(desk->window), root);

    *out_desk = desk;
    return umi_gtk4_desk_refresh(desk);
}

void umi_gtk4_desk_destroy(UmiGtk4Desk *desk)
{
    free(desk);
}

UmiStatus umi_gtk4_desk_refresh(UmiGtk4Desk *desk)
{
    UmiDeskRuntimeSnapshot snapshot;
    UmiDesktopApplicationStrip *strip;
    size_t index;
    UmiStatus status;
    char status_text[256U];
    if (desk == NULL || desk->runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_desk_runtime_refresh(desk->runtime);
    if (status != UMI_STATUS_OK) return status;
    status = umi_desk_runtime_snapshot(desk->runtime, &snapshot);
    if (status != UMI_STATUS_OK) return status;

    strip = umi_desk_runtime_application_strip(desk->runtime);
    clear_box(desk->application_strip);
    for (index = 0U; index < snapshot.strip.item_count; ++index) {
        UmiDesktopApplicationStripItem item;
        status = umi_desktop_application_strip_at(strip, index, &item);
        if (status != UMI_STATUS_OK) return status;
        gtk_box_append(GTK_BOX(desk->application_strip),
                       make_application_button(desk, &item));
        if (item.active) {
            gtk_label_set_text(GTK_LABEL(desk->workbench_title),
                               item.display_name);
            gtk_label_set_text(
                GTK_LABEL(desk->workbench_description),
                item.running
                    ? "Application is active. Framework panels and linked "
                      "contexts can be composed into this workbench."
                    : "Application is selected but not running.");
        }
    }

    clear_box(desk->layout_strip);
    if (snapshot.has_shell) {
        UmiDesktopShellModel *shell = umi_desk_runtime_shell(desk->runtime);
        for (index = 0U; index < snapshot.shell.tab_count; ++index) {
            UmiDesktopShellTab tab;
            status = umi_desktop_shell_model_tab_at(shell, index, &tab);
            if (status != UMI_STATUS_OK) return status;
            gtk_box_append(GTK_BOX(desk->layout_strip),
                           make_layout_button(desk, &tab));
        }
        {
            GtkWidget *new_layout = gtk_button_new_with_label("+");
            gtk_widget_add_css_class(new_layout, "flat");
            gtk_widget_set_tooltip_text(
                new_layout, "Create a new semantic layout");
            gtk_box_append(GTK_BOX(desk->layout_strip), new_layout);
        }
    }

    (void)snprintf(status_text, sizeof(status_text),
                   "%zu applications · %zu running · %zu pinned%s",
                   snapshot.strip.item_count,
                   snapshot.strip.running_count,
                   snapshot.strip.pinned_count,
                   snapshot.strip.attention_count > 0U
                       ? " · attention required" : "");
    set_status(desk, status_text);
    return UMI_STATUS_OK;
}

UmiStatus umi_gtk4_desk_present(UmiGtk4Desk *desk)
{
    if (desk == NULL || desk->window == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    gtk_window_present(GTK_WINDOW(desk->window));
    return UMI_STATUS_OK;
}

void *umi_gtk4_desk_native_window(UmiGtk4Desk *desk)
{
    return desk != NULL ? desk->window : NULL;
}
