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
    UmiGtk4WorkspaceLayoutActionHandler action_handler;
    void *action_user_data;
    size_t placeholder_count;
    uint64_t revision;
};

/*
 * Provide the dispatch panel action operation used by this module and its client
 * applications.
 */
static void dispatch_panel_action(UmiWsPanelAction action,
                                  const UmiWsPanelChrome *chrome,
                                  void *user_data)
{
    UmiGtk4WorkspaceLayoutHost *host =
        (UmiGtk4WorkspaceLayoutHost *)user_data;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || host->action_handler == NULL || chrome == NULL ||
        chrome->panel_id[0] == '\0')
        return;
    host->action_handler(chrome->panel_id, action, host->action_user_data);
}

/*
 * Initialise gtk4 workspace layout placeholder from caller-provided values so later
 * operations receive a known state.
 */
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

/* Provide the clear root operation used by this module and its client applications. */
static void clear_root(GtkWidget *root)
{
    GtkWidget *child;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || !GTK_IS_BOX(root)) return;
    child = gtk_widget_get_first_child(root);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(root), child);
        child = next;
    }
}

/* Provide the build stack operation used by this module and its client applications. */
static GtkWidget *build_stack(UmiGtk4WorkspaceLayoutHost *host,
                              const UmiApplicationSuiteLayoutRenderStack *stack)
{
    GtkWidget *tabs = umi_gtk4_ws_tab_host_create(NULL);
    size_t index;
    gtk_widget_set_hexpand(tabs, TRUE);
    gtk_widget_set_vexpand(tabs, TRUE);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < stack->window_count; ++index) {
        const UmiUiWorkspaceWindow *window =
            &host->layout.windows[stack->window_indices[index]];
        GtkWidget *content = host->panel_factory != NULL
            ? host->panel_factory(window, host->user_data) : NULL;
        UmiWsPanelChrome chrome;
        GtkWidget *frame;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (content == NULL) {
            content = umi_gtk4_workspace_layout_placeholder_create(
                window->title,
                "The canonical panel is part of this layout, but its product "
                "renderer or capability is not available yet.");
            host->placeholder_count += 1U;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_ws_panel_chrome_init(&chrome, window->title) != UMI_STATUS_OK)
            return tabs;
        chrome.show_close = window->closable && !window->pinned;
        chrome.show_pin = true;
        /* The suite workstation now completes these semantic actions through
         * one shared panel editor. Applications receive identical controls
         * without embedding product-specific docking or grouping code. */
        chrome.show_menu = false;
        chrome.show_context = true;
        chrome.show_move = true;
        chrome.show_maximise = true;
        chrome.show_settings = true;
        umi_ws_panel_chrome_set_compact(&chrome, true);
        chrome.pinned = window->pinned;
        chrome.locked = host->layout.locked;
        chrome.floating = window->floating;
        chrome.maximised = window->maximised;
        (void)umi_ws_panel_chrome_set_identity(
            &chrome,
            window->window_id,
            window->placement_id[0] != '\0'
                ? window->placement_id
                : umi_ui_placement_text(stack->placement));
        (void)umi_ws_panel_chrome_set_context(
            &chrome,
            window->context_group_id,
            window->context_group_id);
        (void)umi_ws_panel_chrome_set_badge(
            &chrome, umi_ui_placement_text(stack->placement));
        frame = host->action_handler != NULL
            ? umi_gtk4_ws_panel_frame_create_interactive(
                  &chrome, content, dispatch_panel_action, host)
            : umi_gtk4_ws_panel_frame_create(&chrome, content);
        (void)umi_gtk4_ws_tab_host_append(tabs, window->title, frame);
    }
    return tabs;
}

/* Provide the build region operation used by this module and its client applications. */
static GtkWidget *build_region(UmiGtk4WorkspaceLayoutHost *host,
                               UmiUiPlacement placement)
{
    GtkWidget *region = NULL;
    size_t occurrence;
    size_t count = umi_application_suite_layout_render_plan_count_placement(
        &host->plan, placement);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count == 0U) return NULL;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count == 1U) {
        return build_stack(
            host,
            umi_application_suite_layout_render_plan_find_placement(
                &host->plan, placement, 0U));
    }
    region = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    /* Visit each bounded item once so every record receives the same rule. */
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

/* Provide the split widgets operation used by this module and its client applications. */
static GtkWidget *split_widgets(const char *split_id,
                                UmiUiOrientation orientation,
                                double ratio,
                                GtkWidget *first,
                                GtkWidget *second)
{
    UmiWsSplitRegion split;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (first == NULL) return second;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (second == NULL) return first;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_ws_split_region_init(&split, split_id, orientation, ratio) !=
        UMI_STATUS_OK)
        return first;
    return umi_gtk4_ws_split_host_create(&split, orientation, first, second);
}

/* Provide the build content operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Initialise gtk4 workspace layout host from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_gtk4_workspace_layout_host_create(
    const UmiUiWorkspaceLayout *layout,
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory,
    void *user_data,
    UmiGtk4WorkspaceLayoutHost **out_host)
{
    return umi_gtk4_workspace_layout_host_create_interactive(
        layout, panel_factory, user_data, NULL, NULL, out_host);
}

/*
 * Provide the gtk4 workspace layout host create interactive operation used by this module
 * and its client applications.
 */
UmiStatus umi_gtk4_workspace_layout_host_create_interactive(
    const UmiUiWorkspaceLayout *layout,
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory,
    void *panel_user_data,
    UmiGtk4WorkspaceLayoutActionHandler action_handler,
    void *action_user_data,
    UmiGtk4WorkspaceLayoutHost **out_host)
{
    UmiGtk4WorkspaceLayoutHost *host;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || out_host == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_host = NULL;
    host = (UmiGtk4WorkspaceLayoutHost *)calloc(1U, sizeof(*host));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    host->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_object_ref_sink(host->root);
    host->panel_factory = panel_factory;
    host->user_data = panel_user_data;
    host->action_handler = action_handler;
    host->action_user_data = action_user_data;
    status = umi_gtk4_workspace_layout_host_rebuild(host, layout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_gtk4_workspace_layout_host_destroy(host);
        return status;
    }
    *out_host = host;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by gtk4 workspace layout host so the same storage can be
 * reused safely.
 */
void umi_gtk4_workspace_layout_host_destroy(UmiGtk4WorkspaceLayoutHost *host)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host->root != NULL) g_object_unref(host->root);
    free(host);
}

/*
 * Provide the gtk4 workspace layout host rebuild operation used by this module and its
 * client applications.
 */
UmiStatus umi_gtk4_workspace_layout_host_rebuild(
    UmiGtk4WorkspaceLayoutHost *host,
    const UmiUiWorkspaceLayout *layout)
{
    GtkWidget *content;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || layout == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_suite_layout_render_plan_build(layout, &host->plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    host->layout = *layout;
    host->placeholder_count = 0U;
    clear_root(host->root);
    content = build_content(host);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (content == NULL) return UMI_STATUS_INVALID_STATE;
    gtk_box_append(GTK_BOX(host->root), content);
    host->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the gtk4 workspace layout host widget operation used by this module and its
 * client applications.
 */
GtkWidget *umi_gtk4_workspace_layout_host_widget(
    UmiGtk4WorkspaceLayoutHost *host)
{
    return host != NULL ? host->root : NULL;
}

/*
 * Provide the gtk4 workspace layout host snapshot operation used by this module and its
 * client applications.
 */
UmiGtk4WorkspaceLayoutHostSnapshot umi_gtk4_workspace_layout_host_snapshot(
    const UmiGtk4WorkspaceLayoutHost *host)
{
    UmiGtk4WorkspaceLayoutHostSnapshot snapshot;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
