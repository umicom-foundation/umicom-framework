/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/panel_frame_gtk4.c
 *
 * PURPOSE:
 *   Render Framework-owned panel chrome with complete docking actions, compact
 *   normal-mode presentation and explicit edit-mode controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/panel_frame.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/gtk4/automation.h"

/* Per-button action data owned by its GTK widget. */
typedef struct PanelActionData {
    UmiGtk4WsPanelActionHandler handler;
    void *user_data;
    UmiWsPanelAction action;
    UmiWsPanelChrome chrome;
} PanelActionData;

/* Deferred copy used while a panel action may rebuild its owning widget tree. */
typedef struct PanelPendingAction {
    UmiGtk4WsPanelActionHandler handler;
    void *user_data;
    UmiWsPanelAction action;
    UmiWsPanelChrome chrome;
} PanelPendingAction;

/* Release one signal closure using GTK's exact notifier signature. */
static void panel_action_data_destroy(gpointer data, GClosure *closure)
{
    (void)closure;
    g_free(data);
}

/* Dispatch after the button signal returns so layout rebuilding never destroys
 * a widget while GTK is still emitting that widget's click signal. */
static gboolean dispatch_action_from_idle(gpointer user_data)
{
    PanelPendingAction *pending = (PanelPendingAction *)user_data;

    if (pending != NULL && pending->handler != NULL) {
        pending->handler(
            pending->action, &pending->chrome, pending->user_data);
    }
    return G_SOURCE_REMOVE;
}

/* Convert semantic actions to stable automation suffixes. */
static const char *action_token(UmiWsPanelAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
    case UMI_WS_PANEL_ACTION_PIN_TOGGLE: return "pin";
    case UMI_WS_PANEL_ACTION_CONTEXT_GROUP: return "context";
    case UMI_WS_PANEL_ACTION_MOVE: return "move";
    case UMI_WS_PANEL_ACTION_FLOAT_TOGGLE: return "float";
    case UMI_WS_PANEL_ACTION_MAXIMISE_TOGGLE: return "maximise";
    case UMI_WS_PANEL_ACTION_SETTINGS: return "settings";
    case UMI_WS_PANEL_ACTION_CLOSE: return "close";
    default: return "unknown";
    }
}

/* Route the exact action and panel identity back to the layout owner. */
static void on_action_clicked(GtkButton *button, gpointer user_data)
{
    PanelActionData *data = (PanelActionData *)user_data;
    PanelPendingAction *pending;

    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (data == NULL || data->handler == NULL) return;
    pending = g_new0(PanelPendingAction, 1);
    if (pending == NULL) return;
    pending->handler = data->handler;
    pending->user_data = data->user_data;
    pending->action = data->action;
    pending->chrome = data->chrome;
    if (g_idle_add_full(
            G_PRIORITY_DEFAULT_IDLE,
            dispatch_action_from_idle,
            pending,
            g_free) == 0U) {
        g_free(pending);
    }
}

/* Create common action data and attach it to one button. */
static bool bind_action(
    GtkWidget *button,
    const UmiWsPanelChrome *chrome,
    UmiWsPanelAction action,
    UmiGtk4WsPanelActionHandler handler,
    void *user_data)
{
    PanelActionData *data;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (button == NULL || chrome == NULL) return false;
    if (handler == NULL) return true;
    data = g_new0(PanelActionData, 1);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (data == NULL) return false;
    data->handler = handler;
    data->user_data = user_data;
    data->action = action;
    data->chrome = *chrome;
    g_signal_connect_data(
        button, "clicked", G_CALLBACK(on_action_clicked), data,
        panel_action_data_destroy, 0);
    return true;
}

/* Create one compact icon button while preserving the semantic action name in
 * its tooltip and automation identifier. */
static GtkWidget *make_action_button(
    const char *icon_name,
    const UmiWsPanelChrome *chrome,
    UmiWsPanelAction action,
    bool enabled,
    UmiGtk4WsPanelActionHandler handler,
    void *user_data)
{
    GtkWidget *button = gtk_button_new_from_icon_name(icon_name);
    char automation_id[UMI_UI_TEXT_CAPACITY + 32U];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (button == NULL || chrome == NULL) return button;
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-panel-action");
    gtk_widget_set_tooltip_text(button, umi_ws_panel_action_text(action));
    gtk_widget_set_sensitive(button, enabled && handler != NULL);
    (void)snprintf(
        automation_id, sizeof(automation_id), "%s.action.%s",
        chrome->panel_id, action_token(action));
    (void)umi_gtk4_automation_tag_widget(button, automation_id);
    (void)bind_action(
        button, chrome, action, handler, user_data);
    return button;
}

/* Create a labelled overflow-menu action. Normal mode keeps these capabilities
 * discoverable without dedicating one permanent icon to every action. */
static GtkWidget *make_menu_action_button(
    const char *icon_name,
    const UmiWsPanelChrome *chrome,
    UmiWsPanelAction action,
    bool enabled,
    UmiGtk4WsPanelActionHandler handler,
    void *user_data)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *icon = gtk_image_new_from_icon_name(icon_name);
    GtkWidget *label = gtk_label_new(umi_ws_panel_action_text(action));
    char automation_id[UMI_UI_TEXT_CAPACITY + 32U];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (button == NULL || row == NULL || icon == NULL ||
        label == NULL || chrome == NULL) {
        return button;
    }
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-panel-menu-action");
    gtk_widget_set_hexpand(button, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_box_append(GTK_BOX(row), icon);
    gtk_box_append(GTK_BOX(row), label);
    gtk_button_set_child(GTK_BUTTON(button), row);
    gtk_widget_set_tooltip_text(button, umi_ws_panel_action_text(action));
    gtk_widget_set_sensitive(button, enabled && handler != NULL);
    (void)snprintf(
        automation_id, sizeof(automation_id), "%s.menu.%s",
        chrome->panel_id, action_token(action));
    (void)umi_gtk4_automation_tag_widget(button, automation_id);
    (void)bind_action(
        button, chrome, action, handler, user_data);
    return button;
}

/* Append one menu action only when its semantic capability is present. */
static size_t append_menu_action(
    GtkWidget *menu_box,
    const char *icon_name,
    const UmiWsPanelChrome *chrome,
    UmiWsPanelAction action,
    bool visible,
    bool enabled,
    UmiGtk4WsPanelActionHandler handler,
    void *user_data)
{
    GtkWidget *button;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (menu_box == NULL || !visible) return 0U;
    button = make_menu_action_button(
        icon_name, chrome, action, enabled, handler, user_data);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (button == NULL) return 0U;
    gtk_box_append(GTK_BOX(menu_box), button);
    return 1U;
}

/* Create the normal-mode overflow menu. Settings remains available while the
 * layout is locked; geometry-changing actions explain their disabled state
 * through the existing action tooltips and become active in edit mode. */
static GtkWidget *make_overflow_menu(
    const UmiWsPanelChrome *chrome,
    bool editing_enabled,
    UmiGtk4WsPanelActionHandler handler,
    void *user_data)
{
    GtkWidget *button;
    GtkWidget *popover;
    GtkWidget *menu_box;
    size_t action_count = 0U;
    char automation_id[UMI_UI_TEXT_CAPACITY + 32U];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome == NULL) return NULL;
    button = gtk_menu_button_new();
    popover = gtk_popover_new();
    menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (button == NULL || popover == NULL || menu_box == NULL) {
        return button;
    }

    action_count += append_menu_action(
        menu_box, "view-pin-symbolic", chrome,
        UMI_WS_PANEL_ACTION_PIN_TOGGLE,
        chrome->show_pin, editing_enabled, handler, user_data);
    action_count += append_menu_action(
        menu_box, "view-restore-symbolic", chrome,
        UMI_WS_PANEL_ACTION_MOVE,
        chrome->show_move, editing_enabled, handler, user_data);
    action_count += append_menu_action(
        menu_box, "window-new-symbolic", chrome,
        UMI_WS_PANEL_ACTION_FLOAT_TOGGLE,
        chrome->show_float, editing_enabled, handler, user_data);
    action_count += append_menu_action(
        menu_box, "view-fullscreen-symbolic", chrome,
        UMI_WS_PANEL_ACTION_MAXIMISE_TOGGLE,
        chrome->show_maximise, editing_enabled, handler, user_data);
    action_count += append_menu_action(
        menu_box, "emblem-system-symbolic", chrome,
        UMI_WS_PANEL_ACTION_SETTINGS,
        chrome->show_settings, true, handler, user_data);

    /* No empty menu button should occupy permanent panel-header space. */
    if (action_count == 0U) {
        g_object_ref_sink(menu_box);
        g_object_unref(menu_box);
        g_object_ref_sink(popover);
        g_object_unref(popover);
        g_object_ref_sink(button);
        g_object_unref(button);
        return NULL;
    }

    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-panel-action");
    gtk_menu_button_set_icon_name(
        GTK_MENU_BUTTON(button), "view-more-symbolic");
    gtk_widget_set_tooltip_text(button, "Panel actions");
    gtk_popover_set_child(GTK_POPOVER(popover), menu_box);
    gtk_menu_button_set_popover(GTK_MENU_BUTTON(button), popover);
    (void)snprintf(
        automation_id, sizeof(automation_id), "%s.action.menu",
        chrome->panel_id);
    (void)umi_gtk4_automation_tag_widget(button, automation_id);
    return button;
}

/* Only known semantic colour names may become CSS classes. This avoids
 * treating untrusted layout text as a GTK selector while still showing linked
 * panel groups with a consistent colour stripe. */
static const char *context_colour_css_class(const char *colour_token)
{
    static const char *const colours[] = {
        "red", "orange", "yellow", "green",
        "cyan", "blue", "purple", "magenta"
    };
    static const char *const classes[] = {
        "umicom-context-red", "umicom-context-orange",
        "umicom-context-yellow", "umicom-context-green",
        "umicom-context-cyan", "umicom-context-blue",
        "umicom-context-purple", "umicom-context-magenta"
    };
    size_t index;

    if (colour_token == NULL || colour_token[0] == '\0') return NULL;
    for (index = 0U; index < sizeof(colours) / sizeof(colours[0]); ++index) {
        char class_token[64U];
        char semantic_token[64U];

        (void)snprintf(
            class_token, sizeof(class_token), "umicom-context-%s",
            colours[index]);
        (void)snprintf(
            semantic_token, sizeof(semantic_token),
            "umicom.context.colour.%s", colours[index]);
        if (strcmp(colour_token, colours[index]) == 0 ||
            strcmp(colour_token, class_token) == 0 ||
            strcmp(colour_token, semantic_token) == 0) {
            return classes[index];
        }
    }
    return NULL;
}

/* Update linked-context colour after creation without exposing GTK details to
 * callers that own the routing model. */
void umi_gtk4_ws_panel_frame_set_context_colour(
    GtkWidget *frame,
    const char *colour_token)
{
    static const char *const classes[] = {
        "umicom-context-red", "umicom-context-orange",
        "umicom-context-yellow", "umicom-context-green",
        "umicom-context-cyan", "umicom-context-blue",
        "umicom-context-purple", "umicom-context-magenta"
    };
    const char *selected;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (frame == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(classes) / sizeof(classes[0]); ++index) {
        gtk_widget_remove_css_class(frame, classes[index]);
    }
    selected = context_colour_css_class(colour_token);
    if (selected != NULL) gtk_widget_add_css_class(frame, selected);
}

/* Build reusable chrome around caller-supplied content. The content becomes
 * parent-owned by the returned frame. */
GtkWidget *umi_gtk4_ws_panel_frame_create(
    const UmiWsPanelChrome *chrome,
    GtkWidget *content)
{
    return umi_gtk4_ws_panel_frame_create_interactive(
        chrome, content, NULL, NULL);
}

/* Build reusable interactive chrome around caller-supplied content. */
GtkWidget *umi_gtk4_ws_panel_frame_create_interactive(
    const UmiWsPanelChrome *chrome,
    GtkWidget *content,
    UmiGtk4WsPanelActionHandler action_handler,
    void *user_data)
{
    GtkWidget *frame;
    GtkWidget *header;
    GtkWidget *title;
    GtkWidget *badge;
    GtkWidget *context;
    GtkWidget *button;
    GtkWidget *overflow;
    bool editing_enabled;
    char automation_id[UMI_UI_TEXT_CAPACITY + 32U];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome == NULL || content == NULL) return NULL;
    frame = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    header = gtk_box_new(
        GTK_ORIENTATION_HORIZONTAL, chrome->compact ? 2 : 6);
    title = gtk_label_new(chrome->title);
    badge = gtk_label_new(chrome->badge);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (frame == NULL || header == NULL || title == NULL || badge == NULL)
        return NULL;

    editing_enabled = !chrome->locked;
    gtk_widget_add_css_class(frame, "umicom-panel-frame");
    gtk_widget_add_css_class(header, "umicom-panel-header");
    gtk_widget_add_css_class(title, "umicom-panel-title");
    gtk_widget_add_css_class(badge, "umicom-panel-badge");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_label_set_ellipsize(
        GTK_LABEL(title), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(title, TRUE);
    gtk_widget_set_visible(badge, chrome->badge[0] != '\0');
    gtk_box_append(GTK_BOX(header), title);
    gtk_box_append(GTK_BOX(header), badge);

    /* Keep the linked-context control visible because it changes the panel's
     * operational context rather than its geometry. */
    if (action_handler != NULL && chrome->show_context) {
        context = make_action_button(
            "view-filter-symbolic",
            chrome,
            UMI_WS_PANEL_ACTION_CONTEXT_GROUP,
            editing_enabled,
            action_handler,
            user_data);
        gtk_widget_add_css_class(context, "umicom-panel-context-link");
        gtk_box_append(GTK_BOX(header), context);
    }

    /* Normal mode uses one overflow control. Edit mode expands all geometry
     * actions so panel movement remains direct and discoverable. */
    if (action_handler != NULL && chrome->show_menu) {
        overflow = make_overflow_menu(
            chrome, editing_enabled, action_handler, user_data);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (overflow != NULL) gtk_box_append(GTK_BOX(header), overflow);
    } else if (action_handler != NULL) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (chrome->show_pin) {
            button = make_action_button(
                "view-pin-symbolic", chrome,
                UMI_WS_PANEL_ACTION_PIN_TOGGLE, editing_enabled,
                action_handler, user_data);
            gtk_box_append(GTK_BOX(header), button);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (chrome->show_move) {
            button = make_action_button(
                "view-restore-symbolic", chrome,
                UMI_WS_PANEL_ACTION_MOVE, editing_enabled,
                action_handler, user_data);
            gtk_box_append(GTK_BOX(header), button);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (chrome->show_float) {
            button = make_action_button(
                "window-new-symbolic", chrome,
                UMI_WS_PANEL_ACTION_FLOAT_TOGGLE, editing_enabled,
                action_handler, user_data);
            gtk_box_append(GTK_BOX(header), button);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (chrome->show_maximise) {
            button = make_action_button(
                "view-fullscreen-symbolic", chrome,
                UMI_WS_PANEL_ACTION_MAXIMISE_TOGGLE, editing_enabled,
                action_handler, user_data);
            gtk_box_append(GTK_BOX(header), button);
        }
        /* Settings opens the complete editor rather than mutating one field. */
        if (chrome->show_settings) {
            button = make_action_button(
                "emblem-system-symbolic", chrome,
                UMI_WS_PANEL_ACTION_SETTINGS, true,
                action_handler, user_data);
            gtk_box_append(GTK_BOX(header), button);
        }
    }
    /* Closing changes layout membership and is therefore enabled only while
     * the layout owns an active edit transaction. */
    if (action_handler != NULL && chrome->show_close) {
        button = make_action_button(
            "window-close-symbolic", chrome,
            UMI_WS_PANEL_ACTION_CLOSE, editing_enabled,
            action_handler, user_data);
        gtk_box_append(GTK_BOX(header), button);
    }

    gtk_box_append(GTK_BOX(frame), header);
    gtk_box_append(GTK_BOX(frame), content);
    umi_gtk4_ws_panel_frame_set_context_colour(
        frame, chrome->context_colour_token);
    (void)snprintf(
        automation_id, sizeof(automation_id), "%s.frame",
        chrome->panel_id);
    (void)umi_gtk4_automation_tag_widget(frame, automation_id);
    return frame;
}
