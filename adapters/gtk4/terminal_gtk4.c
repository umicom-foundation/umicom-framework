/*-----------------------------------------------------------------------------
 * Umicom Framework GTK4 terminal presentation adapter.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

#include <stdio.h>
#include <string.h>

typedef struct UmiGtk4TerminalAction {
    UmiGtk4Adapter *adapter;
    char action_id[UMI_UI_ID_CAPACITY];
} UmiGtk4TerminalAction;

static void action_free(gpointer data, GClosure *closure)
{
    (void)closure;
    g_free(data);
}

static void action_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4TerminalAction *action = (UmiGtk4TerminalAction *)user_data;
    (void)button;
    if (action != NULL) umi_gtk4_dispatch_action(action->adapter, action->action_id);
}

static int64_t integer_property(const UmiUiViewPresentation *presentation,
                                const char *key)
{
    UmiUiPropertySnapshot property;
    if (umi_ui_view_presentation_find_property(presentation, key, &property) ==
        UMI_STATUS_OK && property.value.kind == UMI_UI_VALUE_INTEGER) {
        return property.value.integer_value;
    }
    return 0;
}

static const char *string_property(const UmiUiViewPresentation *presentation,
                                   const char *key)
{
    UmiUiPropertySnapshot property;
    if (umi_ui_view_presentation_find_property(presentation, key, &property) ==
        UMI_STATUS_OK && property.value.kind == UMI_UI_VALUE_STRING) {
        return property.value.string_value;
    }
    return "";
}

static int boolean_property(const UmiUiViewPresentation *presentation,
                            const char *key)
{
    UmiUiPropertySnapshot property;
    if (umi_ui_view_presentation_find_property(presentation, key, &property) ==
        UMI_STATUS_OK && property.value.kind == UMI_UI_VALUE_BOOLEAN) {
        return property.value.boolean_value;
    }
    return 0;
}

static void append_action(UmiGtk4Adapter *adapter,
                          GtkWidget *box,
                          const char *label,
                          const char *action_id)
{
    UmiGtk4TerminalAction *binding = g_new0(UmiGtk4TerminalAction, 1);
    GtkWidget *button;
    if (binding == NULL) return;
    binding->adapter = adapter;
    (void)g_strlcpy(binding->action_id, action_id, sizeof(binding->action_id));
    button = gtk_button_new_with_label(label);
    gtk_widget_add_css_class(button, "flat");
    g_signal_connect_data(button, "clicked", G_CALLBACK(action_clicked),
                          binding, action_free, 0);
    gtk_box_append(GTK_BOX(box), button);
}

GtkWidget *umi_gtk4_terminal_widget(UmiGtk4Adapter *adapter,
                                    const UmiUiViewPresentation *presentation)
{
    GtkWidget *root;
    GtkWidget *header;
    GtkWidget *title;
    GtkWidget *scroll;
    GtkWidget *tabs;
    GtkWidget *text_view;
    GtkTextBuffer *buffer;
    const char *kind;
    const char *row_prefix;
    const char *count_key;
    int64_t count;
    int64_t index;
    if (adapter == NULL || presentation == NULL) return NULL;
    kind = string_property(presentation, "umicom.view-kind");
    row_prefix = strcmp(kind, "terminal-history") == 0 ? "history.row" : "terminal.row";
    count_key = strcmp(kind, "terminal-history") == 0 ? "history.row-count" : "terminal.row-count";
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    title = gtk_label_new(string_property(presentation, "title"));
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_widget_add_css_class(title, "heading");
    gtk_widget_set_hexpand(title, TRUE);
    gtk_box_append(GTK_BOX(header), title);
    if (strcmp(kind, "terminal-history") == 0) {
        append_action(adapter, header, "Clear History",
                      "studio.action.terminal.history-clear");
    } else {
        append_action(adapter, header, "Previous", "studio.action.terminal.previous");
        append_action(adapter, header, "Next", "studio.action.terminal.next");
        append_action(adapter, header, "New", "studio.action.terminal.new");
        append_action(adapter, header, "Split", "studio.action.terminal.split");
        append_action(adapter, header, "Execute…", "studio.action.terminal.execute");
        append_action(adapter, header, "Search…", "studio.action.terminal.search");
        append_action(adapter, header, "Clear", "studio.action.terminal.clear");
        append_action(adapter, header, "Close", "studio.action.terminal.close");
    }
    gtk_box_append(GTK_BOX(root), header);
    if (strcmp(kind, "terminal-history") != 0) {
        int64_t tab_count = integer_property(presentation, "terminal.tabs");
        tabs = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
        for (index = 0; index < tab_count; ++index) {
            char title_key[64];
            char active_key[64];
            GtkWidget *tab;
            (void)snprintf(title_key, sizeof(title_key),
                           "terminal.tab.%lld.title", (long long)index);
            (void)snprintf(active_key, sizeof(active_key),
                           "terminal.tab.%lld.active", (long long)index);
            tab = gtk_label_new(string_property(presentation, title_key));
            gtk_widget_set_margin_top(tab, 4);
            gtk_widget_set_margin_bottom(tab, 4);
            gtk_widget_set_margin_start(tab, 8);
            gtk_widget_set_margin_end(tab, 8);
            gtk_widget_add_css_class(tab,
                boolean_property(presentation, active_key) ? "accent" : "dim-label");
            gtk_box_append(GTK_BOX(tabs), tab);
        }
        gtk_box_append(GTK_BOX(root), tabs);
    }
    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), TRUE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_NONE);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    count = integer_property(presentation, count_key);
    for (index = 0; index < count; ++index) {
        char key[64];
        UmiUiPropertySnapshot property;
        GtkTextIter end;
        (void)snprintf(key, sizeof(key), "%s.%lld", row_prefix, (long long)index);
        if (umi_ui_view_presentation_find_property(presentation, key, &property) !=
            UMI_STATUS_OK || property.value.kind != UMI_UI_VALUE_STRING) continue;
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_insert(buffer, &end, property.value.string_value, -1);
        gtk_text_buffer_insert(buffer, &end, "\n", 1);
    }
    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), text_view);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(root), scroll);
    return root;
}
