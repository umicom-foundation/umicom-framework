/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/command_bar_gtk4.c
 *
 * PURPOSE:
 *   Render the portable workstation command catalogue as a responsive GTK4
 *   search component shared by Studio, Trader and other suite applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/command_bar.h"

#include <stdlib.h>
#include <string.h>

#define UMI_GTK4_COMMAND_BAR_DEFAULT_RESULTS 10U

struct UmiGtk4WorkstationCommandBar {
    UmiWsCommandBarModel model;
    GtkWidget *root;
    GtkWidget *scope_label;
    GtkWidget *entry;
    GtkWidget *result_button;
    GtkWidget *popover;
    GtkWidget *result_list;
    UmiGtk4WorkstationCommandBarActivatedHandler activated_handler;
    void *activated_user_data;
    char placeholder[UMI_UI_TEXT_CAPACITY];
    char compact_placeholder[UMI_UI_TEXT_CAPACITY];
    size_t maximum_visible_results;
    int changing_text;
    uint64_t revision;
};

/* Return a short label that explains how the current prefix narrows results. */
static const char *scope_text(UmiWsCommandScope scope)
{
    switch (scope) {
    case UMI_WS_COMMAND_SCOPE_COMMAND: return "Commands";
    case UMI_WS_COMMAND_SCOPE_SYMBOL: return "Symbols";
    case UMI_WS_COMMAND_SCOPE_TEXT: return "Text";
    case UMI_WS_COMMAND_SCOPE_LINE: return "Lines";
    case UMI_WS_COMMAND_SCOPE_SETTING: return "Settings";
    case UMI_WS_COMMAND_SCOPE_PANEL: return "Windows";
    case UMI_WS_COMMAND_SCOPE_AI: return "Assistant";
    default: return "All";
    }
}

/* Remove old rows before a new query is projected. GTK owns every child after
 * append, so removing it from the list also releases the old row safely. */
static void clear_result_list(GtkWidget *list)
{
    GtkWidget *child;

    if (list == NULL) return;
    child = gtk_widget_get_first_child(list);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(list), child);
        child = next;
    }
}

/* Find an item by copied identifier instead of storing a pointer in a button.
 * This remains safe when an application replaces the whole catalogue model. */
static const UmiWsCommandBarItem *find_item(
    const UmiGtk4WorkstationCommandBar *command_bar,
    const char *item_id)
{
    size_t index;

    if (command_bar == NULL || item_id == NULL) return NULL;
    for (index = 0U; index < command_bar->model.count; ++index) {
        if (strcmp(command_bar->model.items[index].item_id, item_id) == 0) {
            return &command_bar->model.items[index];
        }
    }
    return NULL;
}

/* Dispatch only enabled entries. Disabled entries remain visible so a user
 * can discover an action even when its current context prevents execution. */
static void on_result_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar =
        (UmiGtk4WorkstationCommandBar *)user_data;
    const char *item_id;
    const UmiWsCommandBarItem *item;

    if (command_bar == NULL || button == NULL) return;
    item_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-command-bar-item-id");
    item = find_item(command_bar, item_id);
    if (item == NULL || !item->enabled) return;

    if (command_bar->activated_handler != NULL) {
        command_bar->activated_handler(
            item, command_bar->activated_user_data);
    }
    gtk_popover_popdown(GTK_POPOVER(command_bar->popover));
}

/* Project portable results into native rows. Only a bounded number is rendered
 * while the model continues to report the complete number of matches. */
static void rebuild_result_widgets(
    UmiGtk4WorkstationCommandBar *command_bar)
{
    size_t limit;
    size_t index;

    if (command_bar == NULL || command_bar->result_list == NULL) return;
    clear_result_list(command_bar->result_list);
    gtk_label_set_text(
        GTK_LABEL(command_bar->scope_label),
        scope_text(command_bar->model.query.scope));

    limit = command_bar->model.result_count;
    if (limit > command_bar->maximum_visible_results) {
        limit = command_bar->maximum_visible_results;
    }
    for (index = 0U; index < limit; ++index) {
        const UmiWsCommandBarItem *item =
            umi_ws_command_bar_model_result_at(&command_bar->model, index);
        GtkWidget *button;
        GtkWidget *content;
        GtkWidget *title;
        GtkWidget *description;

        if (item == NULL) continue;
        button = gtk_button_new();
        content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        title = gtk_label_new(item->title);
        description = gtk_label_new(item->description);
        if (button == NULL || content == NULL || title == NULL ||
            description == NULL) {
            continue;
        }

        gtk_widget_add_css_class(button, "umicom-command-bar-result");
        gtk_widget_add_css_class(description, "dim-label");
        gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
        gtk_label_set_ellipsize(
            GTK_LABEL(description), PANGO_ELLIPSIZE_END);
        gtk_box_append(GTK_BOX(content), title);
        gtk_box_append(GTK_BOX(content), description);
        gtk_button_set_child(GTK_BUTTON(button), content);
        gtk_widget_set_sensitive(button, item->enabled);
        gtk_widget_set_tooltip_text(button, item->description);
        g_object_set_data_full(
            G_OBJECT(button),
            "umicom-command-bar-item-id",
            g_strdup(item->item_id),
            g_free);
        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(on_result_clicked),
            command_bar);
        gtk_list_box_append(GTK_LIST_BOX(command_bar->result_list), button);
    }
    ++command_bar->revision;
}

/* Re-run portable filtering whenever keyboard input changes. The results
 * button owns the popover, keeping it anchored to the header during resize. */
static void on_search_changed(GtkSearchEntry *entry, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar =
        (UmiGtk4WorkstationCommandBar *)user_data;

    if (command_bar == NULL || command_bar->changing_text) return;
    (void)umi_ws_command_bar_model_set_query(
        &command_bar->model,
        gtk_editable_get_text(GTK_EDITABLE(entry)));
    rebuild_result_widgets(command_bar);
}

/* Enter activates the highest-priority matching action. Every visible result
 * remains clickable when the user needs a different action with similar text. */
static void on_search_activate(GtkSearchEntry *entry, gpointer user_data)
{
    UmiGtk4WorkstationCommandBar *command_bar =
        (UmiGtk4WorkstationCommandBar *)user_data;
    const UmiWsCommandBarItem *item;
    (void)entry;

    if (command_bar == NULL) return;
    item = umi_ws_command_bar_model_selected(&command_bar->model);
    if (item == NULL || !item->enabled ||
        command_bar->activated_handler == NULL) {
        return;
    }
    command_bar->activated_handler(item, command_bar->activated_user_data);
    gtk_popover_popdown(GTK_POPOVER(command_bar->popover));
}

/* Keep actions visible on small screens by shortening the field before it can
 * push the central editor or chart workspace beyond the monitor edge. */
static void apply_presentation(UmiGtk4WorkstationCommandBar *command_bar)
{
    UmiWsCommandBarPresentation presentation;

    if (command_bar == NULL) return;
    presentation = command_bar->model.presentation;
    gtk_widget_set_visible(
        command_bar->scope_label,
        presentation == UMI_WS_COMMAND_BAR_PRESENTATION_EXPANDED);
    gtk_widget_set_visible(
        command_bar->entry,
        presentation != UMI_WS_COMMAND_BAR_PRESENTATION_BUTTON);
    if (presentation == UMI_WS_COMMAND_BAR_PRESENTATION_EXPANDED) {
        gtk_entry_set_placeholder_text(
            GTK_ENTRY(command_bar->entry), command_bar->placeholder);
        gtk_editable_set_width_chars(GTK_EDITABLE(command_bar->entry), 24);
        gtk_menu_button_set_icon_name(
            GTK_MENU_BUTTON(command_bar->result_button),
            "pan-down-symbolic");
    } else if (presentation == UMI_WS_COMMAND_BAR_PRESENTATION_COMPACT) {
        gtk_entry_set_placeholder_text(
            GTK_ENTRY(command_bar->entry), command_bar->compact_placeholder);
        gtk_editable_set_width_chars(GTK_EDITABLE(command_bar->entry), 10);
        gtk_menu_button_set_icon_name(
            GTK_MENU_BUTTON(command_bar->result_button),
            "system-search-symbolic");
    } else {
        gtk_menu_button_set_label(
            GTK_MENU_BUTTON(command_bar->result_button), "Commands");
    }
    ++command_bar->revision;
}

UmiGtk4WorkstationCommandBarConfig
umi_gtk4_ws_command_bar_config_default(void)
{
    UmiGtk4WorkstationCommandBarConfig config = {0};
    config.placeholder = "Search commands, windows and settings";
    config.compact_placeholder = "Search";
    config.maximum_visible_results = UMI_GTK4_COMMAND_BAR_DEFAULT_RESULTS;
    config.initial_available_width = 320;
    return config;
}

UmiStatus umi_gtk4_ws_command_bar_create_managed(
    const UmiGtk4WorkstationCommandBarConfig *config,
    const UmiWsCommandBarModel *model,
    UmiGtk4WorkstationCommandBar **out_command_bar)
{
    UmiGtk4WorkstationCommandBar *command_bar;
    GtkWidget *popover_root;
    GtkWidget *help;
    UmiStatus status;

    if (config == NULL || model == NULL || out_command_bar == NULL ||
        config->initial_available_width < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_command_bar = NULL;
    command_bar = (UmiGtk4WorkstationCommandBar *)calloc(
        1U, sizeof(*command_bar));
    if (command_bar == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    command_bar->model = *model;
    command_bar->maximum_visible_results =
        config->maximum_visible_results > 0U
            ? config->maximum_visible_results
            : UMI_GTK4_COMMAND_BAR_DEFAULT_RESULTS;
    status = umi_ws_copy_text(
        command_bar->placeholder,
        sizeof(command_bar->placeholder),
        config->placeholder != NULL
            ? config->placeholder
            : "Search commands, windows and settings");
    if (status == UMI_STATUS_OK) {
        status = umi_ws_copy_text(
            command_bar->compact_placeholder,
            sizeof(command_bar->compact_placeholder),
            config->compact_placeholder != NULL
                ? config->compact_placeholder
                : "Search");
    }
    if (status != UMI_STATUS_OK) {
        free(command_bar);
        return status;
    }

    command_bar->root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    command_bar->scope_label = gtk_label_new("All");
    command_bar->entry = gtk_search_entry_new();
    command_bar->result_button = gtk_menu_button_new();
    command_bar->popover = gtk_popover_new();
    command_bar->result_list = gtk_list_box_new();
    popover_root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    help = gtk_label_new(
        "Use > for commands, + for windows, / for settings or ? for the assistant.");
    if (command_bar->root == NULL || command_bar->scope_label == NULL ||
        command_bar->entry == NULL || command_bar->result_button == NULL ||
        command_bar->popover == NULL || command_bar->result_list == NULL ||
        popover_root == NULL || help == NULL) {
        if (command_bar->root != NULL) g_object_ref_sink(command_bar->root);
        umi_gtk4_ws_command_bar_destroy(command_bar);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    g_object_ref_sink(command_bar->root);

    gtk_widget_add_css_class(command_bar->root, "umicom-command-bar-shell");
    gtk_widget_add_css_class(command_bar->scope_label, "dim-label");
    gtk_widget_add_css_class(command_bar->entry, "umicom-command-bar");
    gtk_widget_add_css_class(help, "dim-label");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_label_set_xalign(GTK_LABEL(help), 0.0F);
    gtk_widget_set_margin_top(popover_root, 8);
    gtk_widget_set_margin_bottom(popover_root, 8);
    gtk_widget_set_margin_start(popover_root, 8);
    gtk_widget_set_margin_end(popover_root, 8);
    gtk_widget_set_size_request(popover_root, 420, -1);
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(command_bar->result_list), GTK_SELECTION_NONE);
    gtk_box_append(GTK_BOX(popover_root), help);
    gtk_box_append(GTK_BOX(popover_root), command_bar->result_list);
    gtk_popover_set_child(GTK_POPOVER(command_bar->popover), popover_root);
    gtk_menu_button_set_popover(
        GTK_MENU_BUTTON(command_bar->result_button), command_bar->popover);
    gtk_widget_set_tooltip_text(
        command_bar->result_button, "Show matching commands and actions");
    gtk_box_append(GTK_BOX(command_bar->root), command_bar->scope_label);
    gtk_box_append(GTK_BOX(command_bar->root), command_bar->entry);
    gtk_box_append(GTK_BOX(command_bar->root), command_bar->result_button);

    g_signal_connect(
        command_bar->entry,
        "search-changed",
        G_CALLBACK(on_search_changed),
        command_bar);
    g_signal_connect(
        command_bar->entry,
        "activate",
        G_CALLBACK(on_search_activate),
        command_bar);
    (void)umi_ws_command_bar_model_set_available_width(
        &command_bar->model, config->initial_available_width);
    rebuild_result_widgets(command_bar);
    apply_presentation(command_bar);
    *out_command_bar = command_bar;
    return UMI_STATUS_OK;
}

void umi_gtk4_ws_command_bar_destroy(
    UmiGtk4WorkstationCommandBar *command_bar)
{
    if (command_bar == NULL) return;
    if (command_bar->root != NULL) g_object_unref(command_bar->root);
    free(command_bar);
}

GtkWidget *umi_gtk4_ws_command_bar_widget(
    UmiGtk4WorkstationCommandBar *command_bar)
{
    return command_bar != NULL ? command_bar->root : NULL;
}

UmiStatus umi_gtk4_ws_command_bar_set_model(
    UmiGtk4WorkstationCommandBar *command_bar,
    const UmiWsCommandBarModel *model)
{
    if (command_bar == NULL || model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    command_bar->model = *model;
    rebuild_result_widgets(command_bar);
    return UMI_STATUS_OK;
}

UmiStatus umi_gtk4_ws_command_bar_set_query_text(
    UmiGtk4WorkstationCommandBar *command_bar,
    const char *text)
{
    UmiStatus status;

    if (command_bar == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ws_command_bar_model_set_query(&command_bar->model, text);
    if (status != UMI_STATUS_OK) return status;
    command_bar->changing_text = 1;
    gtk_editable_set_text(GTK_EDITABLE(command_bar->entry), text);
    command_bar->changing_text = 0;
    rebuild_result_widgets(command_bar);
    return UMI_STATUS_OK;
}

UmiStatus umi_gtk4_ws_command_bar_set_available_width(
    UmiGtk4WorkstationCommandBar *command_bar,
    int32_t available_width)
{
    UmiStatus status;

    if (command_bar == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ws_command_bar_model_set_available_width(
        &command_bar->model, available_width);
    if (status == UMI_STATUS_OK) apply_presentation(command_bar);
    return status;
}

UmiStatus umi_gtk4_ws_command_bar_set_activated_handler(
    UmiGtk4WorkstationCommandBar *command_bar,
    UmiGtk4WorkstationCommandBarActivatedHandler handler,
    void *user_data)
{
    if (command_bar == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    command_bar->activated_handler = handler;
    command_bar->activated_user_data = user_data;
    return UMI_STATUS_OK;
}

UmiGtk4WorkstationCommandBarSnapshot umi_gtk4_ws_command_bar_snapshot(
    const UmiGtk4WorkstationCommandBar *command_bar)
{
    UmiGtk4WorkstationCommandBarSnapshot snapshot = {0};
    const UmiWsCommandBarItem *selected;

    if (command_bar == NULL) return snapshot;
    snapshot.scope = command_bar->model.query.scope;
    snapshot.presentation = command_bar->model.presentation;
    snapshot.item_count = command_bar->model.count;
    snapshot.result_count = command_bar->model.result_count;
    snapshot.revision = command_bar->revision;
    (void)umi_ws_copy_text(
        snapshot.query,
        sizeof(snapshot.query),
        command_bar->model.query.text);
    selected = umi_ws_command_bar_model_selected(&command_bar->model);
    if (selected != NULL) {
        (void)umi_ws_copy_text(
            snapshot.selected_item_id,
            sizeof(snapshot.selected_item_id),
            selected->item_id);
    }
    return snapshot;
}

GtkWidget *umi_gtk4_ws_command_bar_create(const char *placeholder)
{
    GtkWidget *entry = gtk_search_entry_new();
    if (entry == NULL) return NULL;
    gtk_widget_add_css_class(entry, "umicom-command-bar");
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(entry),
        placeholder != NULL
            ? placeholder
            : "Search commands, windows, settings and AI");
    gtk_widget_set_hexpand(entry, TRUE);
    return entry;
}

UmiStatus umi_gtk4_ws_command_bar_query(
    GtkWidget *entry,
    UmiWsCommandBarQuery *out_query)
{
    const char *text;
    if (entry == NULL || !GTK_IS_EDITABLE(entry) || out_query == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    text = gtk_editable_get_text(GTK_EDITABLE(entry));
    return umi_ws_command_bar_parse(text, out_query);
}
