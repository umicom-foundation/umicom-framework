/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/panel_frame_gtk4.c
 *
 * PURPOSE:
 *   Render one consistent professional panel header for every GTK4 Umicom
 *   application and route its buttons back to toolkit-neutral panel actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/panel_frame.h"

typedef struct UmiGtk4WsPanelFrameState {
    UmiWsPanelChrome chrome;
    UmiGtk4WsPanelActionHandler action_handler;
    void *user_data;
} UmiGtk4WsPanelFrameState;

static void on_action_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4WsPanelFrameState *state =
        (UmiGtk4WsPanelFrameState *)user_data;
    UmiWsPanelAction action;
    if (state == NULL || state->action_handler == NULL) return;
    action = (UmiWsPanelAction)GPOINTER_TO_INT(
        g_object_get_data(G_OBJECT(button), "umicom-panel-action"));
    state->action_handler(action, &state->chrome, state->user_data);
}

static GtkWidget *make_action_button(UmiGtk4WsPanelFrameState *state,
                                     UmiWsPanelAction action,
                                     const char *icon_name,
                                     bool enabled)
{
    GtkWidget *button = gtk_button_new_from_icon_name(icon_name);
    if (button == NULL) return NULL;
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-panel-action");
    gtk_widget_set_tooltip_text(button, umi_ws_panel_action_text(action));
    gtk_widget_set_sensitive(button, enabled);
    g_object_set_data(G_OBJECT(button), "umicom-panel-action",
                      GINT_TO_POINTER((int)action));
    g_signal_connect(button, "clicked", G_CALLBACK(on_action_clicked), state);
    return button;
}

static void append_action(GtkWidget *header,
                          UmiGtk4WsPanelFrameState *state,
                          UmiWsPanelAction action,
                          const char *icon_name,
                          bool enabled)
{
    GtkWidget *button = make_action_button(state, action, icon_name, enabled);
    if (button != NULL) gtk_box_append(GTK_BOX(header), button);
}

GtkWidget *umi_gtk4_ws_panel_frame_create_interactive(
    const UmiWsPanelChrome *chrome,
    GtkWidget *child,
    UmiGtk4WsPanelActionHandler action_handler,
    void *user_data)
{
    UmiGtk4WsPanelFrameState *state =
        g_new0(UmiGtk4WsPanelFrameState, 1U);
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *titles = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *title;
    GtkWidget *subtitle;
    GtkWidget *badge;
    bool editing_enabled;

    if (state == NULL || frame == NULL || root == NULL || header == NULL ||
        titles == NULL) {
        g_free(state);
        return frame;
    }
    if (chrome != NULL) state->chrome = *chrome;
    state->action_handler = action_handler;
    state->user_data = user_data;
    g_object_set_data_full(G_OBJECT(frame), "umicom-panel-frame-state", state,
                           g_free);

    title = gtk_label_new(chrome != NULL ? chrome->title : "Panel");
    subtitle = gtk_label_new(chrome != NULL ? chrome->subtitle : "");
    badge = gtk_label_new(chrome != NULL ? chrome->badge : "");
    gtk_widget_add_css_class(frame, "umicom-workstation-panel");
    gtk_widget_add_css_class(header, "umicom-panel-header");
    if (chrome != NULL && chrome->compact)
        gtk_widget_add_css_class(header, "umicom-panel-header-compact");
    if (chrome != NULL && chrome->locked)
        gtk_widget_add_css_class(header, "umicom-panel-header-locked");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_widget_set_hexpand(titles, TRUE);
    gtk_box_append(GTK_BOX(titles), title);
    if (chrome != NULL && chrome->subtitle[0] != '\0') {
        gtk_label_set_xalign(GTK_LABEL(subtitle), 0.0F);
        gtk_widget_add_css_class(subtitle, "dim-label");
        gtk_box_append(GTK_BOX(titles), subtitle);
    }
    gtk_box_append(GTK_BOX(header), titles);
    if (chrome != NULL && chrome->badge[0] != '\0') {
        gtk_widget_add_css_class(badge, "umicom-panel-badge");
        gtk_box_append(GTK_BOX(header), badge);
    }

    editing_enabled = chrome == NULL || !chrome->locked;
    if (action_handler != NULL && chrome != NULL && chrome->show_context)
        append_action(header, state, UMI_WS_PANEL_ACTION_CONTEXT_GROUP,
                      "insert-link-symbolic", editing_enabled);
    if (action_handler != NULL && chrome != NULL && chrome->show_move)
        append_action(header, state, UMI_WS_PANEL_ACTION_MOVE,
                      "transform-move-symbolic", editing_enabled);
    if (action_handler != NULL && chrome != NULL && chrome->show_pin)
        append_action(header, state, UMI_WS_PANEL_ACTION_PIN_TOGGLE,
                      "view-pin-symbolic", editing_enabled);
    if (action_handler != NULL && chrome != NULL && chrome->show_float)
        append_action(header, state, UMI_WS_PANEL_ACTION_FLOAT_TOGGLE,
                      "view-restore-symbolic", editing_enabled);
    if (action_handler != NULL && chrome != NULL && chrome->show_maximise)
        append_action(header, state, UMI_WS_PANEL_ACTION_MAXIMISE_TOGGLE,
                      "view-fullscreen-symbolic", editing_enabled);
    if (action_handler != NULL && chrome != NULL && chrome->show_settings)
        append_action(header, state, UMI_WS_PANEL_ACTION_SETTINGS,
                      "emblem-system-symbolic", true);
    if (action_handler != NULL && chrome != NULL && chrome->show_close)
        append_action(header, state, UMI_WS_PANEL_ACTION_CLOSE,
                      "window-close-symbolic", editing_enabled);

    gtk_box_append(GTK_BOX(root), header);
    if (child != NULL) {
        gtk_widget_set_hexpand(child, TRUE);
        gtk_widget_set_vexpand(child, TRUE);
        gtk_box_append(GTK_BOX(root), child);
    }
    gtk_frame_set_child(GTK_FRAME(frame), root);
    return frame;
}

GtkWidget *umi_gtk4_ws_panel_frame_create(const UmiWsPanelChrome *chrome,
                                          GtkWidget *child)
{
    return umi_gtk4_ws_panel_frame_create_interactive(
        chrome, child, NULL, NULL);
}
