/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/task_gtk4.c
 *
 * PURPOSE:
 *   Implement the task gtk4 behavior for
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
 * Umicom Framework GTK4 task-queue presentation adapter.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

#include <stdio.h>

typedef struct UmiGtk4TaskAction {
    UmiGtk4Adapter *adapter;
    char action_id[UMI_UI_ID_CAPACITY];
} UmiGtk4TaskAction;

/* Provide the task action free operation used by this module and its client applications. */
static void task_action_free(gpointer data, GClosure *closure)
{
    (void)closure;
    g_free(data);
}

/*
 * Provide the task action clicked operation used by this module and its client
 * applications.
 */
static void task_action_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4TaskAction *binding = (UmiGtk4TaskAction *)user_data;
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
 * Provide the append task action operation used by this module and its client
 * applications.
 */
static void append_task_action(UmiGtk4Adapter *adapter,
                               GtkWidget *box,
                               const char *label,
                               const char *action_id)
{
    UmiGtk4TaskAction *binding = g_new0(UmiGtk4TaskAction, 1);
    GtkWidget *button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL) return;
    binding->adapter = adapter;
    (void)g_strlcpy(binding->action_id, action_id, sizeof(binding->action_id));
    button = gtk_button_new_with_label(label);
    g_signal_connect_data(button, "clicked", G_CALLBACK(task_action_clicked),
                          binding, task_action_free, 0);
    gtk_box_append(GTK_BOX(box), button);
}

/* Provide the task integer operation used by this module and its client applications. */
static int64_t task_integer(const UmiUiViewPresentation *presentation,
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

/* Provide the metric card operation used by this module and its client applications. */
static GtkWidget *metric_card(const char *label, int64_t value)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    GtkWidget *number;
    GtkWidget *name;
    char text[64];
    (void)snprintf(text, sizeof(text), "%lld", (long long)value);
    number = gtk_label_new(text);
    gtk_widget_add_css_class(number, "title-1");
    name = gtk_label_new(label);
    gtk_widget_add_css_class(name, "dim-label");
    gtk_box_append(GTK_BOX(box), number);
    gtk_box_append(GTK_BOX(box), name);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    return box;
}

/* Provide the gtk4 task widget operation used by this module and its client applications. */
GtkWidget *umi_gtk4_task_widget(UmiGtk4Adapter *adapter,
                                const UmiUiViewPresentation *presentation)
{
    GtkWidget *root;
    GtkWidget *title;
    GtkWidget *metrics;
    GtkWidget *actions;
    GtkWidget *list;
    GtkWidget *scroll;
    int64_t count;
    int64_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || presentation == NULL) return NULL;
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    title = gtk_label_new("Background Tasks");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_widget_add_css_class(title, "heading");
    gtk_box_append(GTK_BOX(root), title);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    append_task_action(adapter, actions, "Refresh",
                       "studio.action.tasks.refresh");
    append_task_action(adapter, actions, "Wait for Idle",
                       "studio.action.tasks.wait");
    append_task_action(adapter, actions, "Retry…",
                       "studio.action.tasks.retry");
    append_task_action(adapter, actions, "Cancel Build",
                       "studio.action.tasks.cancel");
    gtk_box_append(GTK_BOX(root), actions);
    metrics = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(metrics), metric_card("Queued",
        task_integer(presentation, "task.queued")));
    gtk_box_append(GTK_BOX(metrics), metric_card("Running",
        task_integer(presentation, "task.running")));
    gtk_box_append(GTK_BOX(metrics), metric_card("Completed",
        task_integer(presentation, "task.completed")));
    gtk_box_append(GTK_BOX(metrics), metric_card("Failed",
        task_integer(presentation, "task.failed")));
    gtk_box_append(GTK_BOX(metrics), metric_card("Cancelled",
        task_integer(presentation, "task.cancelled")));
    gtk_box_append(GTK_BOX(root), metrics);
    list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list), GTK_SELECTION_SINGLE);
    count = task_integer(presentation, "operation.row-count");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < count; ++index) {
        char key[64];
        UmiUiPropertySnapshot property;
        GtkWidget *label;
        (void)snprintf(key, sizeof(key), "operation.row.%lld",
                       (long long)index);
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ui_view_presentation_find_property(presentation, key,
                &property) != UMI_STATUS_OK ||
            property.value.kind != UMI_UI_VALUE_STRING) continue;
        label = gtk_label_new(property.value.string_value);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
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
