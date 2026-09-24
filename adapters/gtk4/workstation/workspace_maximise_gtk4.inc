/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_maximise_gtk4.inc
 *
 * PURPOSE:
 *   Expand an existing internal panel temporarily while preserving its widgets,
 *   sibling pages and the canonical saved workspace geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Restore original parentage before destroying wrappers or calling an owner.
 * Holding the frame and normal tree avoids depending on notebook internals. */
static void restore_maximised_presentation(UmiGtk4WorkspaceLayoutHost *host, int focus)
{
    GtkWidget *content;
    GtkWidget *saved_focus;
    if (host == NULL) return;
    if (host->pending_restore_id != 0U) {
        guint source_id = host->pending_restore_id;
        host->pending_restore_id = 0U;
        (void)g_source_remove(source_id);
    }
    if (!host->maximise_mode.active) return;
    /* Retained old controls must not retain callbacks into this host after
     * the temporary wrapper has been removed. */
    g_signal_handlers_disconnect_by_data(host->maximise_restore_button, host);
    g_signal_handlers_disconnect_by_data(host->maximise_keys, host);
    content = gtk_frame_get_child(GTK_FRAME(host->maximise_frame));
    if (content != NULL) {
        g_object_ref(content);
        gtk_frame_set_child(GTK_FRAME(host->maximise_frame), NULL);
        gtk_frame_set_child(GTK_FRAME(host->maximise_source_frame), content);
        g_object_unref(content);
    }
    gtk_widget_set_visible(host->maximise_normal, TRUE);
    gtk_box_remove(GTK_BOX(host->root), host->maximise_container);
    host->maximise_container = NULL;
    host->maximise_frame = NULL;
    host->maximise_restore_button = NULL;
    host->maximise_keys = NULL;
    g_clear_object(&host->maximise_source_frame);
    g_clear_object(&host->maximise_normal);
    saved_focus = host->maximise_focus;
    host->maximise_focus = NULL;
    umi_ws_maximize_mode_restore(&host->maximise_mode);
    update_canvas_controls(host);
    if (saved_focus != NULL) {
        if (focus) (void)gtk_widget_grab_focus(saved_focus);
        g_object_unref(saved_focus);
    }
}

/* Defer removal of Restore controls until their current GTK signal returns. */
static gboolean restore_maximised_from_idle(gpointer user_data)
{
    UmiGtk4WorkspaceLayoutHost *host = user_data;
    host->pending_restore_id = 0U;
    restore_maximised_presentation(host, 1);
    return G_SOURCE_REMOVE;
}

/* Coalesce repeated clicks and Escape presses into one cancellable request. */
static void queue_maximised_restore(UmiGtk4WorkspaceLayoutHost *host)
{
    if (host == NULL || !host->maximise_mode.active || host->pending_restore_id != 0U)
        return;
    host->pending_restore_id = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,
        restore_maximised_from_idle, host, NULL);
}

/* A visible Restore button keeps the transient mode discoverable. */
static void on_maximised_restore_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    queue_maximised_restore(user_data);
}

/* Bubble-phase Escape is a fallback: provider widgets may consume it first.
 * Modifier combinations remain available to editor and application commands. */
static gboolean on_maximised_key_pressed(GtkEventControllerKey *controller,
    guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
    (void)controller;
    (void)keycode;
    if (keyval != GDK_KEY_Escape ||
        (state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_ALT_MASK | GDK_SUPER_MASK |
                  GDK_HYPER_MASK | GDK_META_MASK)) != 0U)
        return FALSE;
    queue_maximised_restore(user_data);
    return TRUE;
}

/* Expand an existing frame child, not a second provider or a copied geometry
 * model. Its original frame remains in the exact notebook/canvas position. */
UmiStatus umi_gtk4_workspace_layout_host_maximise_window(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    const UmiUiWorkspaceWindow *window;
    GtkWidget *source;
    GtkWidget *content;
    GtkWidget *normal;
    GtkWidget *bar;
    GtkWidget *label;
    GtkRoot *root;
    char **classes;
    UmiWsMaximizeMode candidate = {0};
    UmiStatus status;
    if (host == NULL || !umi_ws_id_valid(window_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (host->active_canvas_entry != NULL || host->pending_geometry_id != 0U ||
        host->pending_action_id != 0U || host->pending_restore_id != 0U)
        return UMI_STATUS_BUSY;
    window = umi_ui_workspace_layout_find_window(&host->layout, window_id);
    if (window == NULL || !window->visible || window->floating || tool_window_edge_index(window) >= 0)
        return UMI_STATUS_NOT_FOUND;
    if (window->pinned || !window->resizable) return UMI_STATUS_PERMISSION_DENIED;
    if (umi_ws_maximize_mode_matches(&host->maximise_mode, window_id)) return UMI_STATUS_OK;
    source = find_live_window_frame(host->root, window_id);
    if (source == NULL || !GTK_IS_FRAME(source) || gtk_frame_get_child(GTK_FRAME(source)) == NULL)
        return UMI_STATUS_NOT_FOUND;
    status = umi_ws_maximize_mode_activate(&candidate, window_id);
    if (status != UMI_STATUS_OK) return status;
    /* Validate the requested target before ending a different current mode. */
    restore_maximised_presentation(host, 0);
    normal = gtk_widget_get_first_child(host->root);
    if (normal == NULL) return UMI_STATUS_NOT_FOUND;
    content = gtk_frame_get_child(GTK_FRAME(source));
    root = gtk_widget_get_root(source);
    if (root != NULL) {
        GtkWidget *focused = gtk_root_get_focus(root);
        if (focused != NULL && (focused == content || gtk_widget_is_ancestor(focused, content)))
            host->maximise_focus = g_object_ref(focused);
    }
    host->maximise_source_frame = g_object_ref(source);
    host->maximise_normal = g_object_ref(normal);
    host->maximise_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(host->maximise_container, TRUE);
    gtk_widget_set_vexpand(host->maximise_container, TRUE);
    gtk_widget_add_css_class(host->maximise_container, "umicom-workspace-maximised");
    bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(bar, "umicom-workspace-maximise-bar");
    label = gtk_label_new(window->title[0] != '\0' ? window->title : window->window_id);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_widget_set_hexpand(label, TRUE);
    host->maximise_restore_button = gtk_button_new_with_label("Restore panel");
    (void)umi_gtk4_automation_tag_widget(host->maximise_restore_button,
        "workstation.workspace.restore-maximised");
    gtk_widget_set_tooltip_text(host->maximise_restore_button,
        "Restore the previous workspace view (Escape when not used by the panel)");
    g_signal_connect(host->maximise_restore_button, "clicked",
        G_CALLBACK(on_maximised_restore_clicked), host);
    gtk_box_append(GTK_BOX(bar), label);
    gtk_box_append(GTK_BOX(bar), host->maximise_restore_button);
    gtk_box_append(GTK_BOX(host->maximise_container), bar);
    host->maximise_frame = gtk_frame_new(NULL);
    gtk_widget_set_hexpand(host->maximise_frame, TRUE);
    gtk_widget_set_vexpand(host->maximise_frame, TRUE);
    classes = gtk_widget_get_css_classes(source);
    gtk_widget_set_css_classes(host->maximise_frame, (const char **)classes);
    g_strfreev(classes);
    g_object_ref(content);
    gtk_frame_set_child(GTK_FRAME(source), NULL);
    gtk_frame_set_child(GTK_FRAME(host->maximise_frame), content);
    g_object_unref(content);
    gtk_box_append(GTK_BOX(host->maximise_container), host->maximise_frame);
    host->maximise_keys = gtk_event_controller_key_new();
    gtk_event_controller_set_propagation_phase(host->maximise_keys, GTK_PHASE_BUBBLE);
    g_signal_connect(host->maximise_keys, "key-pressed",
        G_CALLBACK(on_maximised_key_pressed), host);
    gtk_widget_add_controller(host->maximise_container, host->maximise_keys);
    host->maximise_mode = candidate;
    umi_gtk4_workspace_layout_host_collapse_tool_windows(host);
    gtk_widget_set_visible(normal, FALSE);
    gtk_box_append(GTK_BOX(host->root), host->maximise_container);
    update_canvas_controls(host);
    if (host->maximise_focus != NULL)
        (void)gtk_widget_grab_focus(host->maximise_focus);
    else
        (void)gtk_widget_child_focus(host->maximise_frame, GTK_DIR_TAB_FORWARD);
    return UMI_STATUS_OK;
}

/* Public restoration is idempotent, including when an idle restore is queued. */
UmiStatus umi_gtk4_workspace_layout_host_restore_maximised(UmiGtk4WorkspaceLayoutHost *host)
{
    if (host == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    restore_maximised_presentation(host, 1);
    return UMI_STATUS_OK;
}

/* Responsive dock sizing extends the existing split host without replacing it. */
#include "workspace_split_policy_gtk4.inc"
