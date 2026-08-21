/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_gtk4.c
 *
 * PURPOSE:
 *   Compose the GTK4 visual Layout Designer from Framework-owned browser,
 *   toolbar, palette, canvas, tree, property, review and status models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


static GtkWidget *designer_scrolled(GtkWidget *child)
{
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), child);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);
    return scrolled;
}

UmiStatus umi_workbench_designer_gtk4_create(
    const UmiWorkbenchDesignerGtk4Config *config,
    UmiWorkbenchDesignerGtk4 **out_designer)
{
    UmiWorkbenchDesignerGtk4 *designer;
    GtkWidget *body;
    GtkWidget *left_paned;
    GtkWidget *right_paned;
    GtkWidget *centre_box;
    GtkWidget *side_notebook;
    GtkWidget *review_notebook;
    UmiStatus status;
    if (config == NULL || out_designer == NULL || config->controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_designer = NULL;
    designer = g_new0(UmiWorkbenchDesignerGtk4, 1U);
    if (designer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    designer->config = *config;
    umi_workbench_designer_model_bundle_init(&designer->bundle);
    designer->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(designer->root, "umicom-layout-designer");
    designer->toolbar = umi_workbench_designer_gtk4_build_toolbar(designer);
    gtk_box_append(GTK_BOX(designer->root), designer->toolbar);

    status = umi_workbench_layout_browser_gtk4_create(
        config->controller, &designer->browser);
    if (status != UMI_STATUS_OK) {
        umi_workbench_designer_gtk4_destroy(designer);
        return status;
    }
    designer->browser_widget =
        umi_workbench_layout_browser_gtk4_widget(designer->browser);
    designer->browser_revealer = gtk_revealer_new();
    gtk_revealer_set_transition_type(
        GTK_REVEALER(designer->browser_revealer),
        GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN);
    gtk_revealer_set_child(
        GTK_REVEALER(designer->browser_revealer),
        designer->browser_widget);
    gtk_revealer_set_reveal_child(
        GTK_REVEALER(designer->browser_revealer),
        config->show_browser ? TRUE : FALSE);
    gtk_box_append(GTK_BOX(designer->root), designer->browser_revealer);

    body = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(body, TRUE);
    gtk_widget_set_vexpand(body, TRUE);
    designer->work_area = body;
    left_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    right_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(left_paned, TRUE);
    gtk_widget_set_vexpand(left_paned, TRUE);
    gtk_widget_set_hexpand(right_paned, TRUE);
    gtk_widget_set_vexpand(right_paned, TRUE);

    designer->palette_box = umi_workbench_designer_gtk4_build_palette(designer);
    gtk_widget_set_size_request(designer->palette_box, 220, -1);
    gtk_paned_set_start_child(
        GTK_PANED(left_paned), designer_scrolled(designer->palette_box));

    centre_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    designer->canvas_overlay = umi_workbench_designer_gtk4_build_canvas(designer);
    gtk_box_append(GTK_BOX(centre_box), designer->canvas_overlay);
    gtk_paned_set_start_child(GTK_PANED(right_paned), centre_box);

    side_notebook = gtk_notebook_new();
    gtk_widget_set_size_request(side_notebook, 300, -1);
    designer->tree_box = umi_workbench_designer_gtk4_build_tree(designer);
    designer->properties_box = umi_workbench_designer_gtk4_build_properties(designer);
    gtk_notebook_append_page(
        GTK_NOTEBOOK(side_notebook),
        designer_scrolled(designer->tree_box),
        gtk_label_new("Layout"));
    gtk_notebook_append_page(
        GTK_NOTEBOOK(side_notebook),
        designer_scrolled(designer->properties_box),
        gtk_label_new("Properties"));

    review_notebook = umi_workbench_designer_gtk4_build_review(designer);
    gtk_notebook_append_page(
        GTK_NOTEBOOK(side_notebook),
        review_notebook,
        gtk_label_new("Review"));
    gtk_paned_set_end_child(GTK_PANED(right_paned), side_notebook);
    gtk_paned_set_end_child(GTK_PANED(left_paned), right_paned);
    gtk_box_append(GTK_BOX(body), left_paned);
    gtk_box_append(GTK_BOX(designer->root), body);

    designer->command_popover =
        umi_workbench_designer_gtk4_build_command_palette(designer);
    gtk_box_append(
        GTK_BOX(designer->root),
        umi_workbench_designer_gtk4_build_status(designer));
    *out_designer = designer;
    return umi_workbench_designer_gtk4_refresh(designer, 0U);
}

void umi_workbench_designer_gtk4_destroy(
    UmiWorkbenchDesignerGtk4 *designer)
{
    if (designer == NULL) return;
    if (designer->browser != NULL) {
        umi_workbench_layout_browser_gtk4_destroy(designer->browser);
        designer->browser = NULL;
    }
    g_free(designer);
}

GtkWidget *umi_workbench_designer_gtk4_widget(
    UmiWorkbenchDesignerGtk4 *designer)
{
    return designer != NULL ? designer->root : NULL;
}

UmiStatus umi_workbench_designer_gtk4_refresh(
    UmiWorkbenchDesignerGtk4 *designer,
    uint64_t timestamp_ms)
{
    UmiStatus status;
    if (designer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_designer_model_bundle_capture(
        &designer->bundle,
        designer->config.controller,
        designer->config.keymap,
        timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    umi_workbench_designer_gtk4_refresh_toolbar(designer);
    umi_workbench_designer_gtk4_refresh_palette(designer);
    umi_workbench_designer_gtk4_refresh_tree(designer);
    umi_workbench_designer_gtk4_refresh_properties(designer);
    umi_workbench_designer_gtk4_refresh_review(designer);
    umi_workbench_designer_gtk4_refresh_status(designer);
    umi_workbench_designer_gtk4_refresh_command_palette(designer);
    gtk_widget_queue_draw(designer->canvas_area);
    if (designer->minimap_area != NULL) {
        gtk_widget_queue_draw(designer->minimap_area);
    }
    designer->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_gtk4_show_browser(
    UmiWorkbenchDesignerGtk4 *designer,
    bool visible)
{
    if (designer == NULL || designer->browser_revealer == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    gtk_revealer_set_reveal_child(
        GTK_REVEALER(designer->browser_revealer),
        visible ? TRUE : FALSE);
    designer->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_gtk4_focus_canvas(
    UmiWorkbenchDesignerGtk4 *designer)
{
    if (designer == NULL || designer->canvas_area == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return gtk_widget_grab_focus(designer->canvas_area)
        ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}

void umi_workbench_designer_gtk4_dispatch(
    UmiWorkbenchDesignerGtk4 *designer,
    const char *command_id)
{
    if (designer == NULL || command_id == NULL || command_id[0] == '\0') return;
    if (designer->config.command_handler != NULL) {
        designer->config.command_handler(
            command_id,
            designer->bundle.service_snapshot.active_session_id,
            designer->config.command_user_data);
    }
}

void umi_workbench_designer_gtk4_clear_box(GtkWidget *box)
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
