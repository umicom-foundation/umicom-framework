/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/view_model_panel_gtk4.c
 *
 * PURPOSE:
 *   Render Framework view-model metadata, rows and command affordances into a
 *   reusable GTK4 workstation panel while keeping the model toolkit-neutral.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/view_model_panel.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/chart/plot.h"
#include "umicom/ui/command_view.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/workstation/chart_surface.h"
#include "umicom/ui/property.h"
#include "umicom/ui/workstation/chart_surface.h"

#define UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY 768U

typedef struct ActionBinding {
    UmiGtk4ViewModelActionHandler handler;
    void *user_data;
    char action_id[UMI_UI_ID_CAPACITY];
} ActionBinding;

/* Release or reset state held by action binding so the same storage can be reused safely. */
static void action_binding_destroy(gpointer data, GClosure *closure)
{
    (void)closure;
    free(data);
}

/* Provide the on action clicked operation used by this module and its client applications. */
static void on_action_clicked(GtkButton *button, gpointer data)
{
    ActionBinding *binding = (ActionBinding *)data;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding != NULL && binding->handler != NULL)
        (void)binding->handler(binding->action_id, binding->user_data);
}

/* Provide the value text operation used by this module and its client applications. */
static void value_text(const UmiUiValue *value, char *text, size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || capacity == 0U) return;
    text[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    /* Select the behaviour associated with the requested command or state value. */
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

/* Provide the read string operation used by this module and its client applications. */
static const char *read_string(UmiUiViewModel *view, const char *key,
                               UmiUiValue *storage, const char *fallback)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_view_model_get_property(view, key, storage) == UMI_STATUS_OK &&
        storage->kind == UMI_UI_VALUE_STRING && storage->string_value[0] != '\0')
        return storage->string_value;
    return fallback;
}

/* Read one real property while rejecting a missing value or a different value kind. */
static UmiStatus read_real_property(
    UmiUiViewModel *view,
    const char *key,
    double *out_value)
{
    UmiUiValue value;
    UmiStatus status;

    /* The model, key and output are all required for a safe property read. */
    if (view == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ui_view_model_get_property(view, key, &value);
    /* Preserve a missing-property result so the caller can show an empty chart. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* A value with another kind must not be interpreted through the real field. */
    if (value.kind != UMI_UI_VALUE_REAL) {
        return UMI_STATUS_INVALID_STATE;
    }

    *out_value = value.real_value;
    return UMI_STATUS_OK;
}

/* Build a responsive scene from the selected market bar stored in the view model. */
static UmiStatus build_trading_chart_scene(
    UmiUiViewModel *view,
    UmiChartRenderScene **out_scene)
{
    UmiUiValue has_bar;
    UmiChartCandle candle = {0};
    UmiChartPlotViewport viewport;
    UmiChartPlotStyle style;
    UmiChartRenderScene *scene = NULL;
    UmiStatus status;

    /* Clear the output first so every failure leaves an unambiguous empty result. */
    if (view == NULL || out_scene == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_scene = NULL;

    status = umi_ui_view_model_get_property(view, "trading.has-bar", &has_bar);
    /* A missing current bar is a normal empty chart state. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Only a true Boolean value authorises reading the associated OHLC fields. */
    if (has_bar.kind != UMI_UI_VALUE_BOOLEAN || !has_bar.boolean_value) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = read_real_property(view, "trading.open", &candle.open);
    /* Read the high only when the opening value was available and correctly typed. */
    if (status == UMI_STATUS_OK) {
        status = read_real_property(view, "trading.high", &candle.high);
    }
    /* Read the low only when every earlier value in this property group is valid. */
    if (status == UMI_STATUS_OK) {
        status = read_real_property(view, "trading.low", &candle.low);
    }
    /* Read the close only when the partial candle is still valid to complete. */
    if (status == UMI_STATUS_OK) {
        status = read_real_property(view, "trading.close", &candle.close);
    }
    /* Read volume last so no later logic can confuse an incomplete candle with data. */
    if (status == UMI_STATUS_OK) {
        status = read_real_property(view, "trading.volume", &candle.volume);
    }
    /* Do not create a visual from an incomplete property group. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    candle.time_ms = 0;
    status = umi_chart_render_scene_create(32U, &scene);
    /* Preserve allocation failure and leave ownership with no caller. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_chart_render_scene_set_coordinate_size(scene, 640.0, 360.0);
    /* Calculate a padded visible range only after responsive scene setup succeeds. */
    if (status == UMI_STATUS_OK) {
        status = umi_chart_plot_viewport_from_candles(
            &candle,
            1U,
            (UmiChartRenderRectangle){12.0, 12.0, 616.0, 336.0},
            0.08,
            &viewport);
    }
    umi_chart_plot_style_dark(&style);
    /* Keep the application theme visible beneath the chart's grid and data. */
    style.background_color.alpha = 0.0;
    /* Add the shared frame before data so the candle is painted above its grid. */
    if (status == UMI_STATUS_OK) {
        status = umi_chart_plot_add_frame(scene, &viewport, &style);
    }
    /* Add market data only after the complete background and grid were stored. */
    if (status == UMI_STATUS_OK) {
        status = umi_chart_plot_add_candlesticks(
            scene,
            &candle,
            1U,
            &viewport,
            &style);
    }

    /* Release partial scene data when any stage could not finish safely. */
    if (status != UMI_STATUS_OK) {
        umi_chart_render_scene_destroy(scene);
        return status;
    }

    *out_scene = scene;
    return UMI_STATUS_OK;
}

/* Provide the is row property operation used by this module and its client applications. */
static int is_row_property(const char *key)
{
    return key != NULL && strncmp(key, "trading.row.", 12U) == 0;
}

/*
 * Provide the is hidden property operation used by this module and its client
 * applications.
 */
static int is_hidden_property(const char *key)
{
    return key == NULL || strcmp(key, "title") == 0 ||
           strcmp(key, "summary") == 0 ||
           strcmp(key, "umicom.view-kind") == 0 ||
           umi_ui_command_view_property_is_reserved(key);
}

/*
 * Provide the create chart if needed operation used by this module and its client
 * applications.
 */
static GtkWidget *create_chart_if_needed(UmiUiViewModel *view,
                                         const char *view_kind,
                                         const char *title)
{
    UmiWsChartSurface chart = {0};
    UmiChartRenderScene *scene = NULL;
    UmiUiViewSnapshot snapshot;
    GtkWidget *widget;
    UmiStatus scene_status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view_kind == NULL || strcmp(view_kind, "trading-chart") != 0)
        return NULL;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_view_model_snapshot(view, &snapshot) != UMI_STATUS_OK)
        return NULL;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_chart_surface_init(&chart, snapshot.view_id, title) != UMI_STATUS_OK) {
        return NULL;
    }
    chart.show_grid = true;
    chart.sync_symbol = true;
    chart.sync_time = true;
    chart.sync_crosshair = true;

    scene_status = build_trading_chart_scene(view, &scene);
    /* Missing data is a normal state; other failures also retain an honest empty chart. */
    if (scene_status != UMI_STATUS_OK) {
        scene = NULL;
    }

    widget = umi_gtk4_ws_chart_surface_create_with_scene(&chart, scene);
    /* The widget owns a clone, so this temporary scene is always released here. */
    umi_chart_render_scene_destroy(scene);
    return widget;
}

/*
 * Initialise gtk4 view model panel from caller-provided values so later operations receive
 * a known state.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
        UmiUiViewSnapshot view_snapshot;

        /* The view ID lets UAT wait for an entire business panel to appear. */
        if (umi_ui_view_model_snapshot(view, &view_snapshot) == UMI_STATUS_OK) {
            (void)umi_gtk4_automation_tag_widget(
                scroller,
                view_snapshot.view_id);
        }
    }

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
        /* Apply this branch only when its contract condition is satisfied. */
        if (summary[0] != '\0') gtk_box_append(GTK_BOX(content), description);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (chart != NULL) {
            gtk_widget_set_size_request(chart, -1, 220);
            gtk_widget_set_hexpand(chart, TRUE);
            gtk_box_append(GTK_BOX(content), chart);
        }
    }

    properties = umi_ui_view_model_properties(view);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (properties != NULL) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < umi_ui_property_bag_count(properties); ++index) {
            UmiUiPropertySnapshot property;
            char text[UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY];
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (umi_ui_property_bag_at(properties, index, &property) !=
                UMI_STATUS_OK)
                continue;
            /* Apply this branch only when its contract condition is satisfied. */
            if (is_hidden_property(property.key)) continue;
            value_text(&property.value, text, sizeof(text));
            /* Apply this branch only when its contract condition is satisfied. */
            if (is_row_property(property.key)) {
                GtkWidget *row = gtk_label_new(text);
                gtk_label_set_xalign(GTK_LABEL(row), 0.0F);
                gtk_widget_add_css_class(row, "umicom-view-model-row");
                gtk_box_append(GTK_BOX(rows), row);
                row_count += 1;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (metric_row > 0) gtk_box_append(GTK_BOX(content), metrics);
    /* Apply this branch only when its contract condition is satisfied. */
    if (row_count > 0) gtk_box_append(GTK_BOX(content), rows);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_UI_COMMAND_VIEW_ACTION_MAX; ++index) {
        UmiUiCommandViewAction action;
        GtkWidget *button;
        ActionBinding *binding;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_ui_command_view_action_at(view, index, &action) != UMI_STATUS_OK)
            continue;
        button = gtk_button_new_with_label(action.label);
        /* View-model actions carry canonical IDs shared by all native adapters. */
        (void)umi_gtk4_automation_tag_widget(button, action.action_id);
        gtk_widget_set_tooltip_text(button, action.tooltip);
        gtk_widget_set_sensitive(button, action.enabled && action_handler != NULL);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (action_handler != NULL) {
            binding = (ActionBinding *)calloc(1U, sizeof(*binding));
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (action_count > 0) gtk_box_append(GTK_BOX(content), actions);
    return scroller;
}
