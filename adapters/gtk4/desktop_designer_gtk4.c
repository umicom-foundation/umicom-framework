/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desktop_designer_gtk4.c
 *
 * PURPOSE:
 *   Present the Framework visual layout designer as an interactive GTK4
 *   multi-monitor canvas. All selection, snapping, docking and history remain
 *   in the Master Controller-owned desktop model.
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
#include <string.h>

typedef struct UmiGtk4DesktopDesignerControls {
    UmiGtk4Adapter *adapter;
    GtkWidget *canvas;
    double last_drag_x;
    double last_drag_y;
} UmiGtk4DesktopDesignerControls;

/* Provide the refresh desktop operation used by this module and its client applications. */
static void refresh_desktop(UmiGtk4DesktopDesignerControls *controls)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controls != NULL && controls->adapter != NULL)
        (void)umi_gtk4_refresh_workbench(controls->adapter);
}

/* Provide the draw monitor operation used by this module and its client applications. */
static void draw_monitor(cairo_t *context,
                         const UmiDesktopCanvasMonitor *monitor)
{
    cairo_set_source_rgb(context, 0.08, 0.11, 0.14);
    cairo_rectangle(context, monitor->bounds.x, monitor->bounds.y,
                    monitor->bounds.width, monitor->bounds.height);
    cairo_fill_preserve(context);
    /* Apply this branch only when its contract condition is satisfied. */
    if (monitor->primary) cairo_set_source_rgb(context, 0.79, 0.30, 0.33);
    /* Use this fallback path when the earlier condition does not apply. */
    else cairo_set_source_rgb(context, 0.30, 0.40, 0.48);
    cairo_set_line_width(context, monitor->primary ? 3.0 : 1.5);
    cairo_stroke(context);
    cairo_set_source_rgb(context, 0.65, 0.72, 0.78);
    cairo_move_to(context, monitor->bounds.x + 8.0,
                  monitor->bounds.y + 16.0);
    cairo_show_text(context, monitor->name);
}

/* Provide the draw window operation used by this module and its client applications. */
static void draw_window(cairo_t *context,
                        const UmiDesktopCanvasWindow *window)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (window->selected) cairo_set_source_rgba(context, 0.79, 0.30, 0.33, 0.76);
    else /* Apply this branch only when its contract condition is satisfied. */ if (window->locked) cairo_set_source_rgba(context, 0.18, 0.25, 0.31, 0.94);
    /* Use this fallback path when the earlier condition does not apply. */
    else cairo_set_source_rgba(context, 0.20, 0.33, 0.42, 0.94);
    cairo_rectangle(context, window->bounds.x, window->bounds.y,
                    window->bounds.width, window->bounds.height);
    cairo_fill_preserve(context);
    cairo_set_source_rgb(context, window->selected ? 1.0 : 0.48,
                         window->selected ? 0.82 : 0.62,
                         window->selected ? 0.82 : 0.70);
    cairo_set_line_width(context, window->selected ? 3.0 : 1.0);
    cairo_stroke(context);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (window->bounds.width >= 70.0 && window->bounds.height >= 28.0) {
        cairo_save(context);
        cairo_rectangle(context, window->bounds.x + 4.0,
                        window->bounds.y + 4.0,
                        window->bounds.width - 8.0,
                        window->bounds.height - 8.0);
        cairo_clip(context);
        cairo_set_source_rgb(context, 0.93, 0.95, 0.97);
        cairo_move_to(context, window->bounds.x + 8.0,
                      window->bounds.y + 18.0);
        cairo_show_text(context, window->title);
        cairo_restore(context);
    }
}

/* Provide the draw canvas operation used by this module and its client applications. */
static void draw_canvas(GtkDrawingArea *area,
                        cairo_t *context,
                        int width,
                        int height,
                        gpointer user_data)
{
    UmiGtk4DesktopDesignerControls *controls =
        (UmiGtk4DesktopDesignerControls *)user_data;
    UmiDesktopShellSnapshot snapshot;
    size_t index;
    (void)area;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controls == NULL || controls->adapter == NULL ||
        controls->adapter->desktop_shell == NULL) return;
    (void)umi_desktop_shell_model_set_viewport(
        controls->adapter->desktop_shell, (double)width, (double)height);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_desktop_shell_model_snapshot(
            controls->adapter->desktop_shell, &snapshot) != UMI_STATUS_OK)
        return;
    cairo_set_source_rgb(context, 0.055, 0.075, 0.095);
    cairo_paint(context);
    cairo_select_font_face(context, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(context, 11.0);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.monitor_count; ++index) {
        UmiDesktopCanvasMonitor monitor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_desktop_shell_model_monitor_at(
                controls->adapter->desktop_shell, index, &monitor) ==
            UMI_STATUS_OK) draw_monitor(context, &monitor);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.window_count; ++index) {
        UmiDesktopCanvasWindow window;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_desktop_shell_model_window_at(
                controls->adapter->desktop_shell, index, &window) ==
            UMI_STATUS_OK && window.visible) draw_window(context, &window);
    }
}

/* Provide the on canvas pressed operation used by this module and its client applications. */
static void on_canvas_pressed(GtkGestureClick *gesture,
                              int press_count,
                              double x,
                              double y,
                              gpointer user_data)
{
    UmiGtk4DesktopDesignerControls *controls =
        (UmiGtk4DesktopDesignerControls *)user_data;
    (void)gesture;
    (void)press_count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controls == NULL || controls->adapter->desktop_shell == NULL) return;
    (void)umi_desktop_shell_model_select_at(
        controls->adapter->desktop_shell, x, y);
    gtk_widget_queue_draw(controls->canvas);
}

/* Provide the on drag begin operation used by this module and its client applications. */
static void on_drag_begin(GtkGestureDrag *gesture,
                          double start_x,
                          double start_y,
                          gpointer user_data)
{
    UmiGtk4DesktopDesignerControls *controls =
        (UmiGtk4DesktopDesignerControls *)user_data;
    (void)gesture;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controls == NULL || controls->adapter->desktop_shell == NULL) return;
    controls->last_drag_x = 0.0;
    controls->last_drag_y = 0.0;
    (void)umi_desktop_shell_model_select_at(
        controls->adapter->desktop_shell, start_x, start_y);
    gtk_widget_queue_draw(controls->canvas);
}

/* Provide the on drag update operation used by this module and its client applications. */
static void on_drag_update(GtkGestureDrag *gesture,
                           double offset_x,
                           double offset_y,
                           gpointer user_data)
{
    UmiGtk4DesktopDesignerControls *controls =
        (UmiGtk4DesktopDesignerControls *)user_data;
    double delta_x;
    double delta_y;
    (void)gesture;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controls == NULL || controls->adapter->desktop_shell == NULL) return;
    delta_x = offset_x - controls->last_drag_x;
    delta_y = offset_y - controls->last_drag_y;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_desktop_shell_model_move_selected_canvas(
            controls->adapter->desktop_shell, delta_x, delta_y) ==
        UMI_STATUS_OK) {
        controls->last_drag_x = offset_x;
        controls->last_drag_y = offset_y;
        gtk_widget_queue_draw(controls->canvas);
    }
}

/* Provide the on desktop action operation used by this module and its client applications. */
static void on_desktop_action(GtkButton *button, gpointer user_data)
{
    UmiGtk4DesktopDesignerControls *controls =
        (UmiGtk4DesktopDesignerControls *)user_data;
    UmiDesktopShellModel *shell;
    UmiDesktopLayoutDesigner *designer;
    UmiDesktopShellSnapshot snapshot;
    const char *action_id;
    UmiStatus status = UMI_STATUS_NOT_FOUND;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controls == NULL || controls->adapter == NULL) return;
    shell = controls->adapter->desktop_shell;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (shell == NULL) return;
    designer = umi_desktop_shell_model_designer(shell);
    action_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-desktop-action-id");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (action_id == NULL ||
        umi_desktop_shell_model_snapshot(shell, &snapshot) != UMI_STATUS_OK)
        return;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(action_id, UMI_DESKTOP_ACTION_NEW_LAYOUT) == 0) {
        char layout_id[UMI_DESKTOP_ID_CAPACITY];
        char name[UMI_DESKTOP_TITLE_CAPACITY];
        (void)snprintf(layout_id, sizeof(layout_id), "user-%.96s-%llu",
                       snapshot.active_layout_id,
                       (unsigned long long)snapshot.revision);
        (void)snprintf(name, sizeof(name), "%.183s Custom",
                       snapshot.active_layout_name);
        status = umi_desktop_shell_model_begin_design(shell, layout_id, name);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(action_id, UMI_DESKTOP_ACTION_SAVE_LAYOUT) == 0) {
        status = umi_desktop_shell_model_end_design(shell, true);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(action_id, UMI_DESKTOP_ACTION_CANCEL_LAYOUT) == 0) {
        status = umi_desktop_shell_model_end_design(shell, false);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(action_id, UMI_DESKTOP_ACTION_UNDO) == 0) {
        status = umi_desktop_layout_designer_undo(designer);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(action_id, UMI_DESKTOP_ACTION_REDO) == 0) {
        status = umi_desktop_layout_designer_redo(designer);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(action_id, UMI_DESKTOP_ACTION_DOCK_LEFT) == 0) {
        status = umi_desktop_layout_designer_dock_selected(
            designer, UMI_DESKTOP_DOCK_LEFT);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(action_id, UMI_DESKTOP_ACTION_DOCK_RIGHT) == 0) {
        status = umi_desktop_layout_designer_dock_selected(
            designer, UMI_DESKTOP_DOCK_RIGHT);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(action_id, UMI_DESKTOP_ACTION_DOCK_TOP) == 0) {
        status = umi_desktop_layout_designer_dock_selected(
            designer, UMI_DESKTOP_DOCK_TOP);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(action_id, UMI_DESKTOP_ACTION_DOCK_BOTTOM) == 0) {
        status = umi_desktop_layout_designer_dock_selected(
            designer, UMI_DESKTOP_DOCK_BOTTOM);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(action_id, UMI_DESKTOP_ACTION_FLOAT) == 0) {
        status = umi_desktop_layout_designer_dock_selected(
            designer, UMI_DESKTOP_DOCK_FLOATING);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) refresh_desktop(controls);
}

/* Provide the action button operation used by this module and its client applications. */
static GtkWidget *action_button(
    UmiGtk4DesktopDesignerControls *controls,
    const UmiDesktopShellAction *action)
{
    GtkWidget *button = gtk_button_new_with_label(action->label);
    gtk_widget_set_sensitive(button, action->enabled);
    gtk_widget_set_tooltip_text(button, action->tooltip);
    gtk_widget_add_css_class(button, "umicom-desktop-action");
    /* Apply this branch only when its contract condition is satisfied. */
    if (action->destructive)
        gtk_widget_add_css_class(button, "destructive-action");
    g_object_set_data_full(G_OBJECT(button), "umicom-desktop-action-id",
                           g_strdup(action->action_id), g_free);
    g_signal_connect(button, "clicked",
                     G_CALLBACK(on_desktop_action), controls);
    return button;
}

/*
 * Provide the gtk4 desktop designer popover operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_desktop_designer_popover(UmiGtk4Adapter *adapter)
{
    UmiGtk4DesktopDesignerControls *controls;
    UmiDesktopShellSnapshot snapshot;
    GtkWidget *popover;
    GtkWidget *root;
    GtkWidget *heading;
    GtkWidget *summary;
    GtkWidget *primary_actions;
    GtkWidget *dock_actions;
    GtkGesture *click;
    GtkGesture *drag;
    char summary_text[UMI_DESKTOP_DESCRIPTION_CAPACITY];
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->desktop_shell == NULL) return NULL;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_desktop_shell_model_snapshot(
            adapter->desktop_shell, &snapshot) != UMI_STATUS_OK) return NULL;
    controls = g_new0(UmiGtk4DesktopDesignerControls, 1);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controls == NULL) return NULL;
    controls->adapter = adapter;
    popover = gtk_popover_new();
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 7);
    heading = gtk_label_new("Federated Desktop Layout Designer");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_widget_add_css_class(heading, "umicom-desktop-designer-title");
    (void)snprintf(summary_text, sizeof(summary_text),
                   "%s · %zu windows · %zu monitor%s · %s",
                   snapshot.active_layout_name, snapshot.window_count,
                   snapshot.monitor_count,
                   snapshot.monitor_count == 1U ? "" : "s",
                   snapshot.designer_active
                       ? "drag windows, snap and dock"
                       : "protected Framework layout");
    summary = gtk_label_new(summary_text);
    gtk_label_set_xalign(GTK_LABEL(summary), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(summary), TRUE);
    gtk_widget_add_css_class(summary, "umicom-desktop-designer-summary");
    controls->canvas = gtk_drawing_area_new();
    gtk_widget_set_size_request(controls->canvas, 760, 430);
    gtk_widget_add_css_class(controls->canvas, "umicom-desktop-canvas");
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(controls->canvas),
                                   draw_canvas, controls, NULL);
    click = gtk_gesture_click_new();
    g_signal_connect(click, "pressed", G_CALLBACK(on_canvas_pressed), controls);
    gtk_widget_add_controller(controls->canvas,
                              GTK_EVENT_CONTROLLER(click));
    drag = gtk_gesture_drag_new();
    g_signal_connect(drag, "drag-begin", G_CALLBACK(on_drag_begin), controls);
    g_signal_connect(drag, "drag-update", G_CALLBACK(on_drag_update), controls);
    gtk_widget_add_controller(controls->canvas,
                              GTK_EVENT_CONTROLLER(drag));
    primary_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    dock_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_desktop_shell_action_count(); ++index) {
        UmiDesktopShellAction action;
        GtkWidget *button;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_desktop_shell_action_at(
                umi_desktop_shell_model_designer(adapter->desktop_shell),
                index, &action) != UMI_STATUS_OK ||
            strcmp(action.action_id, UMI_DESKTOP_ACTION_MONITORS) == 0)
            continue;
        button = action_button(controls, &action);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index < 5U) gtk_box_append(GTK_BOX(primary_actions), button);
        /* Use this fallback path when the earlier condition does not apply. */
        else gtk_box_append(GTK_BOX(dock_actions), button);
    }
    gtk_box_append(GTK_BOX(root), heading);
    gtk_box_append(GTK_BOX(root), summary);
    gtk_box_append(GTK_BOX(root), controls->canvas);
    gtk_box_append(GTK_BOX(root), primary_actions);
    gtk_box_append(GTK_BOX(root), dock_actions);
    gtk_widget_add_css_class(root, "umicom-desktop-designer");
    gtk_popover_set_child(GTK_POPOVER(popover), root);
    g_object_set_data_full(G_OBJECT(popover),
                           "umicom-desktop-designer-controls",
                           controls, g_free);
    return popover;
}
