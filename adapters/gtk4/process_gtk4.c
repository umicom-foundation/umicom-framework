/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/process_gtk4.c
 *
 * PURPOSE:
 *   Implement the process gtk4 behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework GTK4 supervised-process presentation adapter.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

#include <stdio.h>

typedef struct UmiGtk4ProcessAction {
    UmiGtk4Adapter *adapter;
    char action_id[UMI_UI_ID_CAPACITY];
} UmiGtk4ProcessAction;

/*
 * Provide the process action free operation used by this module and its client
 * applications.
 */
static void process_action_free(gpointer data, GClosure *closure)
{
    (void)closure;
    g_free(data);
}

/*
 * Provide the process action clicked operation used by this module and its client
 * applications.
 */
static void process_action_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4ProcessAction *binding = (UmiGtk4ProcessAction *)user_data;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding != NULL) {
        umi_gtk4_dispatch_action(binding->adapter, binding->action_id);
    }
}

/*
 * Provide the append process action operation used by this module and its client
 * applications.
 */
static void append_process_action(UmiGtk4Adapter *adapter,
                                  GtkWidget *box,
                                  const char *label,
                                  const char *action_id)
{
    UmiGtk4ProcessAction *binding = g_new0(UmiGtk4ProcessAction, 1);
    GtkWidget *button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL) return;
    binding->adapter = adapter;
    (void)g_strlcpy(binding->action_id, action_id,
                    sizeof(binding->action_id));
    button = gtk_button_new_with_label(label);
    g_signal_connect_data(button, "clicked", G_CALLBACK(process_action_clicked),
                          binding, process_action_free, 0);
    gtk_box_append(GTK_BOX(box), button);
}

/* Provide the process integer operation used by this module and its client applications. */
static int64_t process_integer(const UmiUiViewPresentation *presentation,
                               const char *key)
{
    UmiUiPropertySnapshot property;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ui_view_presentation_find_property(presentation, key, &property) ==
        UMI_STATUS_OK && property.value.kind == UMI_UI_VALUE_INTEGER) {
        return property.value.integer_value;
    }
    return 0;
}

/*
 * Provide the gtk4 process widget operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_process_widget(UmiGtk4Adapter *adapter,
                                   const UmiUiViewPresentation *presentation)
{
    GtkWidget *root;
    GtkWidget *summary;
    GtkWidget *actions;
    GtkWidget *list;
    GtkWidget *scroll;
    char text[256];
    int64_t count;
    int64_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || presentation == NULL) return NULL;
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    (void)snprintf(text, sizeof(text),
                   "%lld running  %lld succeeded  %lld failed",
                   (long long)process_integer(presentation, "process.running"),
                   (long long)process_integer(presentation, "process.succeeded"),
                   (long long)process_integer(presentation, "process.failed"));
    summary = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(summary), 0.0F);
    gtk_widget_add_css_class(summary, "heading");
    gtk_box_append(GTK_BOX(root), summary);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    append_process_action(adapter, actions, "Refresh",
                          "studio.action.process.refresh");
    append_process_action(adapter, actions, "Cancel…",
                          "studio.action.process.cancel");
    gtk_box_append(GTK_BOX(root), actions);
    list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list), GTK_SELECTION_SINGLE);
    count = process_integer(presentation, "process.row-count");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < count; ++index) {
        char key[64];
        UmiUiPropertySnapshot property;
        GtkWidget *label;
        (void)snprintf(key, sizeof(key), "process.row.%lld", (long long)index);
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ui_view_presentation_find_property(presentation, key, &property) !=
            UMI_STATUS_OK || property.value.kind != UMI_UI_VALUE_STRING) continue;
        label = gtk_label_new(property.value.string_value);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_label_set_wrap(GTK_LABEL(label), TRUE);
        gtk_widget_set_margin_top(label, 5);
        gtk_widget_set_margin_bottom(label, 5);
        gtk_widget_set_margin_start(label, 8);
        gtk_widget_set_margin_end(label, 8);
        gtk_list_box_append(GTK_LIST_BOX(list), label);
    }
    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(root), scroll);
    return root;
}
