/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/view_model_panel_gtk4.c
 *
 * PURPOSE:
 *   Render Framework view-model metadata, rows and command affordances into a
 *   reusable GTK4 workstation panel while keeping the model toolkit-neutral.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/view_model_panel.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/command_view.h"
#include "umicom/ui/gtk4/workstation/chart_surface.h"
#include "umicom/ui/property.h"
#include "umicom/ui/workstation/chart_surface.h"

#define UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY 768U

typedef struct ActionBinding {
    UmiGtk4ViewModelActionHandler handler;
    void *user_data;
    char action_id[UMI_UI_ID_CAPACITY];
} ActionBinding;

static void action_binding_destroy(gpointer data, GClosure *closure)
{
    (void)closure;
    free(data);
}

static void on_action_clicked(GtkButton *button, gpointer data)
{
    ActionBinding *binding = (ActionBinding *)data;
    (void)button;
    if (binding != NULL && binding->handler != NULL)
        (void)binding->handler(binding->action_id, binding->user_data);
}

static void value_text(const UmiUiValue *value, char *text, size_t capacity)
{
    if (text == NULL || capacity == 0U) return;
    text[0] = '\0';
    if (value == NULL) return;
    switch (value->kind) {
    case UMI_UI_VALUE_BOOLEAN:
        (void)snprintf(text, capacity, "%s",
                       value->boolean_value ? "Yes" : "No");
        break;
    case UMI_UI_VALUE_INTEGER:
        (void)snprintf(text, capacity, "%" PRId64, value->integer_value);
        break;
    case UMI_UI_VALUE_REAL:
        (void)snprintf(text, capacity, "%.6g", value->real_value);
        break;
    case UMI_UI_VALUE_STRING:
        (void)snprintf(text, capacity, "%s", value->string_value);
        break;
    default:
        (void)snprintf(text, capacity, "%s", "—");
        break;
    }
}

static const char *read_string(UmiUiViewModel *view, const char *key,
                               UmiUiValue *storage, const char *fallback)
{
    if (umi_ui_view_model_get_property(view, key, storage) == UMI_STATUS_OK &&
        storage->kind == UMI_UI_VALUE_STRING && storage->string_value[0] != '\0')
        return storage->string_value;
    return fallback;
}

static int is_row_property(const char *key)
{
    return key != NULL && strncmp(key, "trading.row.", 12U) == 0;
}

static int is_hidden_property(const char *key)
{
    return key == NULL || strcmp(key, "title") == 0 ||
           strcmp(key, "summary") == 0 ||
           strcmp(key, "umicom.view-kind") == 0 ||
           umi_ui_command_view_property_is_reserved(key);
}

static GtkWidget *create_chart_if_needed(UmiUiViewModel *view,
                                         const char *view_kind,
                                         const char *title)
{
    UmiWsChartSurface *chart;
    UmiUiViewSnapshot snapshot;
    GtkWidget *widget;
    if (view_kind == NULL || strcmp(view_kind, "trading-chart") != 0)
        return NULL;
    if (umi_ui_view_model_snapshot(view, &snapshot) != UMI_STATUS_OK)
        return NULL;
    chart = (UmiWsChartSurface *)calloc(1U, sizeof(*chart));
    if (chart == NULL) return NULL;
    if (umi_ws_chart_surface_init(chart, snapshot.view_id, title) != UMI_STATUS_OK) {
        free(chart);
        return NULL;
    }
    chart->show_grid = true;
    chart->sync_symbol = true;
    chart->sync_time = true;
    chart->sync_crosshair = true;
    widget = umi_gtk4_ws_chart_surface_create(chart);
    if (widget == NULL) {
        free(chart);
        return NULL;
    }
    /* The existing chart renderer borrows the model pointer for its draw
     * callback, so keep the small presentation model alive with the widget. */
    g_object_set_data_full(G_OBJECT(widget),
                           "umicom-view-model-chart-surface",
                           chart,
                           free);
    return widget;
}

GtkWidget *umi_gtk4_view_model_panel_create(
    UmiUiViewModel *view,
    UmiGtk4ViewModelActionHandler action_handler,
    void *user_data)
{
    GtkWidget *scroller;
    GtkWidget *content;
    GtkWidget *metrics;
    GtkWidget *rows;
    GtkWidget *actions;
    UmiUiPropertyBag *properties;
    UmiUiValue title_value;
    UmiUiValue summary_value;
    UmiUiValue kind_value;
    const char *title;
    const char *summary;
    const char *view_kind;
    size_t index;
    int metric_row = 0;
    int row_count = 0;
    int action_count = 0;

    if (view == NULL) return NULL;
    title = read_string(view, "title", &title_value, "Framework View");
    summary = read_string(view, "summary", &summary_value, "");
    view_kind = read_string(view, "umicom.view-kind", &kind_value, "generic");

    scroller = gtk_scrolled_window_new();
    content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    metrics = gtk_grid_new();
    rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(content, "umicom-view-model-panel");
    gtk_widget_add_css_class(metrics, "umicom-view-model-metrics");
    gtk_widget_add_css_class(rows, "umicom-view-model-rows");
    gtk_widget_add_css_class(actions, "umicom-view-model-actions");
    gtk_grid_set_row_spacing(GTK_GRID(metrics), 4);
    gtk_grid_set_column_spacing(GTK_GRID(metrics), 12);
    gtk_widget_set_hexpand(content, TRUE);
    gtk_widget_set_vexpand(content, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), content);

    {
        GtkWidget *heading = gtk_label_new(title);
        GtkWidget *description = gtk_label_new(summary);
        GtkWidget *chart = create_chart_if_needed(view, view_kind, title);
        gtk_widget_add_css_class(heading, "title-4");
        gtk_widget_add_css_class(description, "dim-label");
        gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
        gtk_label_set_wrap(GTK_LABEL(description), TRUE);
        gtk_box_append(GTK_BOX(content), heading);
        if (summary[0] != '\0') gtk_box_append(GTK_BOX(content), description);
        if (chart != NULL) {
            gtk_widget_set_size_request(chart, -1, 220);
            gtk_widget_set_hexpand(chart, TRUE);
            gtk_box_append(GTK_BOX(content), chart);
        }
    }

    properties = umi_ui_view_model_properties(view);
    if (properties != NULL) {
        for (index = 0U; index < umi_ui_property_bag_count(properties); ++index) {
            UmiUiPropertySnapshot property;
            char text[UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY];
            if (umi_ui_property_bag_at(properties, index, &property) !=
                UMI_STATUS_OK)
                continue;
            if (is_hidden_property(property.key)) continue;
            value_text(&property.value, text, sizeof(text));
            if (is_row_property(property.key)) {
                GtkWidget *row = gtk_label_new(text);
                gtk_label_set_xalign(GTK_LABEL(row), 0.0F);
                gtk_widget_add_css_class(row, "umicom-view-model-row");
                gtk_box_append(GTK_BOX(rows), row);
                row_count += 1;
            } else {
                GtkWidget *key = gtk_label_new(property.key);
                GtkWidget *value = gtk_label_new(text);
                gtk_label_set_xalign(GTK_LABEL(key), 0.0F);
                gtk_label_set_xalign(GTK_LABEL(value), 0.0F);
                gtk_label_set_wrap(GTK_LABEL(value), TRUE);
                gtk_widget_add_css_class(key, "dim-label");
                gtk_widget_set_hexpand(value, TRUE);
                gtk_grid_attach(GTK_GRID(metrics), key, 0, metric_row, 1, 1);
                gtk_grid_attach(GTK_GRID(metrics), value, 1, metric_row, 1, 1);
                metric_row += 1;
            }
        }
    }
    if (metric_row > 0) gtk_box_append(GTK_BOX(content), metrics);
    if (row_count > 0) gtk_box_append(GTK_BOX(content), rows);

    for (index = 0U; index < UMI_UI_COMMAND_VIEW_ACTION_MAX; ++index) {
        UmiUiCommandViewAction action;
        GtkWidget *button;
        ActionBinding *binding;
        if (umi_ui_command_view_action_at(view, index, &action) != UMI_STATUS_OK)
            continue;
        button = gtk_button_new_with_label(action.label);
        gtk_widget_set_tooltip_text(button, action.tooltip);
        gtk_widget_set_sensitive(button, action.enabled && action_handler != NULL);
        if (action_handler != NULL) {
            binding = (ActionBinding *)calloc(1U, sizeof(*binding));
            if (binding != NULL) {
                binding->handler = action_handler;
                binding->user_data = user_data;
                (void)snprintf(binding->action_id, sizeof(binding->action_id),
                               "%s", action.action_id);
                g_signal_connect_data(button, "clicked",
                    G_CALLBACK(on_action_clicked), binding,
                    action_binding_destroy, 0);
            }
        }
        gtk_box_append(GTK_BOX(actions), button);
        action_count += 1;
    }
    if (action_count > 0) gtk_box_append(GTK_BOX(content), actions);
    return scroller;
}
