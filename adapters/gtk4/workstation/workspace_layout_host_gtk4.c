/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_layout_host_gtk4.c
 *
 * PURPOSE:
 *   Render canonical workspace layouts with the established Framework GTK4
 *   panel-frame, tab-host, split-host and dock-overlay primitives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/gtk4/workstation/dock_overlay.h"
#include "umicom/ui/gtk4/workstation/panel_frame.h"
#include "umicom/ui/gtk4/workstation/split_host.h"
#include "umicom/ui/gtk4/workstation/tab_host.h"
#include "umicom/ui/workstation/panel_chrome.h"
#include "umicom/ui/workstation/split_region.h"

struct UmiGtk4WorkspaceLayoutHost {
    GtkWidget *root;
    UmiUiWorkspaceLayout layout;
    UmiApplicationSuiteLayoutRenderPlan plan;
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory;
    void *user_data;
    size_t placeholder_count;
    uint64_t revision;
};

GtkWidget *umi_gtk4_workspace_layout_placeholder_create(
    const char *title,
    const char *message)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *heading = gtk_label_new(title != NULL ? title : "Panel");
    GtkWidget *body = gtk_label_new(
        message != NULL ? message : "No frontend renderer is available.");
    gtk_widget_add_css_class(box, "umicom-workspace-placeholder");
    gtk_widget_add_css_class(heading, "title-4");
    gtk_widget_add_css_class(body, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(body), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(body), TRUE);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), body);
    return box;
}

static void clear_root(GtkWidget *root)
{
    GtkWidget *child;
    if (root == NULL || !GTK_IS_BOX(root)) return;
    child = gtk_widget_get_first_child(root);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(root), child);
        child = next;
    }
}

static GtkWidget *build_stack(UmiGtk4WorkspaceLayoutHost *host,
                              const UmiApplicationSuiteLayoutRenderStack *stack)
{
    GtkWidget *tabs = umi_gtk4_ws_tab_host_create(NULL);
    size_t index;
    gtk_widget_set_hexpand(tabs, TRUE);
    gtk_widget_set_vexpand(tabs, TRUE);
    for (index = 0U; index < stack->window_count; ++index) {
        const UmiUiWorkspaceWindow *window =
            &host->layout.windows[stack->window_indices[index]];
        GtkWidget *content = host->panel_factory != NULL
            ? host->panel_factory(window, host->user_data) : NULL;
        UmiWsPanelChrome chrome;
        GtkWidget *frame;
        if (content == NULL) {
            content = umi_gtk4_workspace_layout_placeholder_create(
                window->title,
                "The canonical panel is part of this layout, but its product "
                "renderer or capability is not available yet.");
            host->placeholder_count += 1U;
        }
        if (umi_ws_panel_chrome_init(&chrome, window->title) != UMI_STATUS_OK)
            return tabs;
        chrome.show_close = window->closable;
        chrome.show_pin = true;
        chrome.show_menu = true;
        umi_ws_panel_chrome_set_compact(&chrome, true);
        (void)umi_ws_panel_chrome_set_badge(
            &chrome, umi_ui_placement_text(stack->placement));
        frame = umi_gtk4_ws_panel_frame_create(&chrome, content);
        (void)umi_gtk4_ws_tab_host_append(tabs, window->title, frame);
    }
    return tabs;
}

static GtkWidget *build_region(UmiGtk4WorkspaceLayoutHost *host,
                               UmiUiPlacement placement)
{
    GtkWidget *region = NULL;
    size_t occurrence;
    size_t count = umi_application_suite_layout_render_plan_count_placement(
        &host->plan, placement);
    if (count == 0U) return NULL;
    if (count == 1U) {
        return build_stack(
            host,
            umi_application_suite_layout_render_plan_find_placement(
                &host->plan, placement, 0U));
    }
    region = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    for (occurrence = 0U; occurrence < count; ++occurrence) {
        GtkWidget *stack = build_stack(
            host,
            umi_application_suite_layout_render_plan_find_placement(
                &host->plan, placement, occurrence));
        gtk_widget_set_vexpand(stack, TRUE);
        gtk_box_append(GTK_BOX(region), stack);
    }
    return region;
}

static GtkWidget *split_widgets(const char *split_id,
                                UmiUiOrientation orientation,
                                double ratio,
                                GtkWidget *first,
                                GtkWidget *second)
{
    UmiWsSplitRegion split;
    if (first == NULL) return second;
    if (second == NULL) return first;
    if (umi_ws_split_region_init(&split, split_id, orientation, ratio) !=
        UMI_STATUS_OK)
        return first;
    return umi_gtk4_ws_split_host_create(&split, orientation, first, second);
}

static GtkWidget *build_content(UmiGtk4WorkspaceLayoutHost *host)
{
    GtkWidget *left = build_region(host, UMI_UI_PLACEMENT_LEFT);
    GtkWidget *centre = build_region(host, UMI_UI_PLACEMENT_CENTRE);
    GtkWidget *right = build_region(host, UMI_UI_PLACEMENT_RIGHT);
    GtkWidget *bottom = build_region(host, UMI_UI_PLACEMENT_BOTTOM);
    GtkWidget *top = build_region(host, UMI_UI_PLACEMENT_TOP);
    GtkWidget *floating = build_region(host, UMI_UI_PLACEMENT_FLOATING);
    GtkWidget *body;
    GtkWidget *content;

    if (centre == NULL)
        centre = umi_gtk4_workspace_layout_placeholder_create(
            host->layout.name, "This layout has no centre panel.");
    body = split_widgets("suite.left-centre", UMI_UI_HORIZONTAL, 0.22,
                         left, centre);
    body = split_widgets("suite.core-right", UMI_UI_HORIZONTAL, 0.76,
                         body, right);
    content = split_widgets("suite.body-bottom", UMI_UI_VERTICAL, 0.72,
                            body, bottom);
    content = split_widgets("suite.top-body", UMI_UI_VERTICAL, 0.18,
                            top, content);
    if (floating != NULL) {
        GtkWidget *overlay = umi_gtk4_ws_dock_overlay_create(content);
        gtk_widget_add_css_class(floating, "umicom-floating-layout-stack");
        gtk_widget_set_halign(floating, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(floating, GTK_ALIGN_CENTER);
        gtk_widget_set_size_request(floating, 720, 480);
        gtk_overlay_add_overlay(GTK_OVERLAY(overlay), floating);
        content = overlay;
    }
    gtk_widget_set_hexpand(content, TRUE);
    gtk_widget_set_vexpand(content, TRUE);
    return content;
}

UmiStatus umi_gtk4_workspace_layout_host_create(
    const UmiUiWorkspaceLayout *layout,
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory,
    void *user_data,
    UmiGtk4WorkspaceLayoutHost **out_host)
{
    UmiGtk4WorkspaceLayoutHost *host;
    UmiStatus status;
    if (layout == NULL || out_host == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_host = NULL;
    host = (UmiGtk4WorkspaceLayoutHost *)calloc(1U, sizeof(*host));
    if (host == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    host->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_object_ref_sink(host->root);
    host->panel_factory = panel_factory;
    host->user_data = user_data;
    status = umi_gtk4_workspace_layout_host_rebuild(host, layout);
    if (status != UMI_STATUS_OK) {
        umi_gtk4_workspace_layout_host_destroy(host);
        return status;
    }
    *out_host = host;
    return UMI_STATUS_OK;
}

void umi_gtk4_workspace_layout_host_destroy(UmiGtk4WorkspaceLayoutHost *host)
{
    if (host == NULL) return;
    if (host->root != NULL) g_object_unref(host->root);
    free(host);
}

UmiStatus umi_gtk4_workspace_layout_host_rebuild(
    UmiGtk4WorkspaceLayoutHost *host,
    const UmiUiWorkspaceLayout *layout)
{
    GtkWidget *content;
    UmiStatus status;
    if (host == NULL || layout == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_suite_layout_render_plan_build(layout, &host->plan);
    if (status != UMI_STATUS_OK) return status;
    host->layout = *layout;
    host->placeholder_count = 0U;
    clear_root(host->root);
    content = build_content(host);
    if (content == NULL) return UMI_STATUS_INVALID_STATE;
    gtk_box_append(GTK_BOX(host->root), content);
    host->revision += 1U;
    return UMI_STATUS_OK;
}

GtkWidget *umi_gtk4_workspace_layout_host_widget(
    UmiGtk4WorkspaceLayoutHost *host)
{
    return host != NULL ? host->root : NULL;
}

UmiGtk4WorkspaceLayoutHostSnapshot umi_gtk4_workspace_layout_host_snapshot(
    const UmiGtk4WorkspaceLayoutHost *host)
{
    UmiGtk4WorkspaceLayoutHostSnapshot snapshot;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    if (host == NULL) return snapshot;
    (void)snprintf(snapshot.layout_id, sizeof(snapshot.layout_id), "%s",
                   host->layout.layout_id);
    snapshot.stack_count = host->plan.stack_count;
    snapshot.panel_count = host->plan.visible_window_count;
    snapshot.placeholder_count = host->placeholder_count;
    snapshot.floating_count = host->plan.floating_window_count;
    snapshot.revision = host->revision;
    return snapshot;
}
