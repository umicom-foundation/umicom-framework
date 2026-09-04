/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/view_model_panel_gtk4.c
 *
 * PURPOSE:
 *   Render toolkit-neutral Umicom view models as useful GTK4 workstation
 *   panels, including status, metrics, rows, tables, charts, technical
 *   disclosure and command actions.
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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/chart/plot.h"
#include "umicom/ui/command_view.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/workstation/chart_surface.h"
#include "umicom/ui/property.h"
#include "umicom/ui/workstation/chart_surface.h"

#define UMI_GTK4_TECHNICAL_PROPERTY_PREFIX "umicom.technical."
#define UMI_GTK4_TABLE_PROPERTY_PREFIX "umicom.table."
#define UMI_GTK4_CHART_PROPERTY_PREFIX "umicom.chart."
#define UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY 768U
#define UMI_GTK4_VIEW_MODEL_TABLE_MAX_COLUMNS 32U
#define UMI_GTK4_VIEW_MODEL_TABLE_MAX_ROWS 256U

typedef struct ViewModelPanelActionClosure {
    UmiGtk4ViewModelActionHandler handler;
    void *user_data;
    char action_id[UMI_UI_ID_CAPACITY];
} ViewModelPanelActionClosure;

/* Release one action closure through GTK's required two-argument notifier
 * signature, keeping signal ownership explicit and type-safe. */
static void action_closure_destroy(gpointer data, GClosure *closure)
{
    (void)closure;
    free(data);
}

/* Attach command identity to each GTK button without exposing GTK in the
 * toolkit-neutral action model. */
static void on_action_clicked(GtkButton *button, gpointer user_data)
{
    ViewModelPanelActionClosure *closure =
        (ViewModelPanelActionClosure *)user_data;

    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (closure != NULL && closure->handler != NULL) {
        (void)closure->handler(closure->action_id, closure->user_data);
    }
}

/* Copy presentation text with explicit truncation and guaranteed termination. */
static void copy_display_text(
    char *buffer,
    size_t capacity,
    const char *text)
{
    size_t length;

    if (buffer == NULL || capacity == 0U) return;
    buffer[0] = '\0';
    if (text == NULL) return;
    length = strlen(text);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(buffer, text, length);
    buffer[length] = '\0';
}

/* Convert one portable value to readable text without exposing its storage. */
static void value_text(
    const UmiUiValue *value,
    char *buffer,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || capacity == 0U) return;
    buffer[0] = '\0';
    if (value == NULL) return;

    /* Select the behaviour associated with the requested command or state value. */
    switch (value->kind) {
    case UMI_UI_VALUE_BOOLEAN:
        copy_display_text(
            buffer, capacity, value->boolean_value ? "Yes" : "No");
        break;
    case UMI_UI_VALUE_INTEGER: {
        char number[64U];
        const int written = snprintf(
            number, sizeof(number), "%" PRId64, value->integer_value);

        if (written >= 0) copy_display_text(buffer, capacity, number);
        break;
    }
    case UMI_UI_VALUE_REAL: {
        char number[64U];
        const int written = snprintf(
            number, sizeof(number), "%.8g", value->real_value);

        if (written >= 0) copy_display_text(buffer, capacity, number);
        break;
    }
    case UMI_UI_VALUE_STRING:
        copy_display_text(buffer, capacity, value->string_value);
        break;
    case UMI_UI_VALUE_NONE:
    default:
        break;
    }
}

/* Return an optional string property through caller-owned temporary storage. */
static const char *string_property(
    const UmiUiViewModel *view,
    const char *key,
    UmiUiValue *storage,
    const char *fallback)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || key == NULL || storage == NULL) return fallback;
    if (umi_ui_view_model_get_property(view, key, storage) == UMI_STATUS_OK &&
        storage->kind == UMI_UI_VALUE_STRING &&
        storage->string_value[0] != '\0') {
        return storage->string_value;
    }
    return fallback;
}

/* Read one integer property while rejecting missing or differently typed data. */
static UmiStatus integer_property(
    const UmiUiViewModel *view,
    const char *key,
    int64_t *out_value)
{
    UmiUiValue value;
    UmiStatus status;

    if (view == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_view_model_get_property(view, key, &value);
    if (status != UMI_STATUS_OK) return status;
    if (value.kind != UMI_UI_VALUE_INTEGER) return UMI_STATUS_INVALID_STATE;
    *out_value = value.integer_value;
    return UMI_STATUS_OK;
}

/* Read a numeric property while accepting the two portable numeric kinds. */
static UmiStatus real_property(
    const UmiUiViewModel *view,
    const char *key,
    double *out_value)
{
    UmiUiValue value;
    UmiStatus status;

    if (view == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_view_model_get_property(view, key, &value);
    if (status != UMI_STATUS_OK) return status;
    if (value.kind == UMI_UI_VALUE_REAL) {
        *out_value = value.real_value;
        return UMI_STATUS_OK;
    }
    if (value.kind == UMI_UI_VALUE_INTEGER) {
        *out_value = (double)value.integer_value;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_INVALID_STATE;
}

/* Check a property namespace without relying on pointer identity. */
static bool property_has_prefix(const char *key, const char *prefix)
{
    return key != NULL && prefix != NULL &&
           strncmp(key, prefix, strlen(prefix)) == 0;
}

/* Rows supplied by the trading projections retain their existing compact list. */
static bool is_row_property(const char *key)
{
    return property_has_prefix(key, "trading.row.");
}

/* Technical properties remain available but do not compete with ordinary
 * product content. */
static bool is_technical_property(const char *key)
{
    return property_has_prefix(key, UMI_GTK4_TECHNICAL_PROPERTY_PREFIX);
}

/* Remove the storage namespace before presenting a technical property label. */
static const char *technical_property_label(const char *key)
{
    return is_technical_property(key)
        ? key + strlen(UMI_GTK4_TECHNICAL_PROPERTY_PREFIX)
        : key;
}

/* Internal fields are consumed by specialised renderers and should not appear
 * again as user-facing metric rows. */
static bool is_hidden_property(const char *key)
{
    if (key == NULL || strcmp(key, "title") == 0 ||
        strcmp(key, "summary") == 0 ||
        strcmp(key, "umicom.view-kind") == 0 ||
        strcmp(key, "State") == 0 ||
        strcmp(key, "Message") == 0 ||
        strcmp(key, "Badge") == 0 ||
        strcmp(key, "Progress percent") == 0) {
        return true;
    }
    return umi_ui_command_view_property_is_reserved(key) != 0;
}

/* Create one aligned metric row for ordinary and diagnostic detail grids. */
static void append_property_row(
    GtkWidget *grid,
    int row,
    const char *label_text,
    const UmiUiValue *value)
{
    GtkWidget *label;
    GtkWidget *content;
    char buffer[UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (grid == NULL || label_text == NULL || value == NULL) return;
    value_text(value, buffer, sizeof(buffer));
    label = gtk_label_new(label_text);
    content = gtk_label_new(buffer);
    if (label == NULL || content == NULL) return;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(content), 0.0F);
    gtk_label_set_selectable(GTK_LABEL(content), TRUE);
    gtk_label_set_wrap(GTK_LABEL(content), TRUE);
    gtk_widget_add_css_class(label, "dim-label");
    gtk_widget_set_hexpand(content, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), content, 1, row, 1, 1);
}

/* Status, message, badge and progress form one product-facing card. They are
 * intentionally separate from low-level diagnostic properties. */
static GtkWidget *create_status_card(const UmiUiViewModel *view)
{
    UmiUiValue state_value;
    UmiUiValue message_value;
    UmiUiValue badge_value;
    UmiUiValue progress_value;
    bool has_state;
    bool has_message;
    bool has_badge;
    bool has_progress;
    GtkWidget *card;
    GtkWidget *heading;
    GtkWidget *badge;
    GtkWidget *state;
    GtkWidget *message;
    GtkWidget *progress;
    char state_buffer[UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY];
    char message_buffer[UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY];
    char badge_buffer[UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY];
    char progress_text[32U];
    int64_t percent = 0;

    if (view == NULL) return NULL;
    has_state = umi_ui_view_model_get_property(
        view, "State", &state_value) == UMI_STATUS_OK;
    has_message = umi_ui_view_model_get_property(
        view, "Message", &message_value) == UMI_STATUS_OK;
    has_badge = umi_ui_view_model_get_property(
        view, "Badge", &badge_value) == UMI_STATUS_OK;
    has_progress = umi_ui_view_model_get_property(
        view, "Progress percent", &progress_value) == UMI_STATUS_OK;
    if (!has_state && !has_message && !has_badge && !has_progress) {
        return NULL;
    }

    value_text(has_state ? &state_value : NULL,
               state_buffer, sizeof(state_buffer));
    value_text(has_message ? &message_value : NULL,
               message_buffer, sizeof(message_buffer));
    value_text(has_badge ? &badge_value : NULL,
               badge_buffer, sizeof(badge_buffer));

    card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    heading = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    state = gtk_label_new(state_buffer);
    badge = gtk_label_new(badge_buffer);
    message = gtk_label_new(message_buffer);
    progress = gtk_progress_bar_new();
    if (card == NULL || heading == NULL || state == NULL ||
        badge == NULL || message == NULL || progress == NULL) {
        return card;
    }

    gtk_widget_add_css_class(card, "umicom-product-status-card");
    gtk_widget_add_css_class(state, "umicom-product-state");
    gtk_widget_add_css_class(badge, "umicom-mode-badge");
    gtk_widget_add_css_class(message, "umicom-product-message");
    gtk_label_set_xalign(GTK_LABEL(state), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(message), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(message), TRUE);
    gtk_widget_set_hexpand(state, TRUE);
    gtk_widget_set_visible(badge, badge_buffer[0] != '\0');
    gtk_widget_set_visible(message, message_buffer[0] != '\0');
    gtk_box_append(GTK_BOX(heading), state);
    gtk_box_append(GTK_BOX(heading), badge);
    gtk_box_append(GTK_BOX(card), heading);
    gtk_box_append(GTK_BOX(card), message);

    if (has_progress && progress_value.kind == UMI_UI_VALUE_INTEGER) {
        percent = progress_value.integer_value;
    } else if (has_progress && progress_value.kind == UMI_UI_VALUE_REAL) {
        percent = (int64_t)progress_value.real_value;
    }
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    (void)snprintf(progress_text, sizeof(progress_text), "%" PRId64 "%%", percent);
    gtk_progress_bar_set_fraction(
        GTK_PROGRESS_BAR(progress), (double)percent / 100.0);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress), TRUE);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), progress_text);
    gtk_widget_set_visible(progress, has_progress);
    gtk_box_append(GTK_BOX(card), progress);
    return card;
}

/* Create a compact empty-state placeholder for view models with no ordinary
 * properties, rows, chart data or commands. */
static GtkWidget *create_empty_state(const UmiUiViewModel *view)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *title = gtk_label_new("Nothing to show yet");
    GtkWidget *description = gtk_label_new(
        "This panel will update when its service provides data.");

    (void)view;
    if (box == NULL || title == NULL || description == NULL) return box;
    gtk_widget_add_css_class(box, "umicom-empty-state");
    gtk_widget_add_css_class(title, "title-4");
    gtk_widget_add_css_class(description, "dim-label");
    gtk_label_set_wrap(GTK_LABEL(description), TRUE);
    gtk_label_set_justify(GTK_LABEL(description), GTK_JUSTIFY_CENTER);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_box_append(GTK_BOX(box), title);
    gtk_box_append(GTK_BOX(box), description);
    return box;
}

/* Release a widget that was created but never adopted by a GTK parent. */
static void release_unparented_widget(GtkWidget *widget)
{
    if (widget == NULL) return;
    g_object_ref_sink(widget);
    g_object_unref(widget);
}

/* Render every command through the stable action callback. */
static size_t append_actions(
    GtkWidget *container,
    const UmiUiViewModel *view,
    UmiGtk4ViewModelActionHandler action_handler,
    void *user_data)
{
    GtkWidget *actions;
    size_t index;
    size_t appended = 0U;

    if (container == NULL || view == NULL) return 0U;
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    if (actions == NULL) return 0U;
    gtk_widget_add_css_class(actions, "umicom-view-model-actions");

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_UI_COMMAND_VIEW_ACTION_MAX; ++index) {
        UmiUiCommandViewAction action;
        ViewModelPanelActionClosure *closure = NULL;
        GtkWidget *button;

        if (umi_ui_command_view_action_at(view, index, &action) !=
            UMI_STATUS_OK) {
            continue;
        }
        button = gtk_button_new_with_label(action.label);
        if (button == NULL) continue;
        (void)umi_gtk4_automation_tag_widget(button, action.action_id);
        gtk_widget_set_tooltip_text(
            button,
            action.tooltip[0] != '\0' ? action.tooltip : action.label);
        gtk_widget_set_sensitive(
            button, action.enabled != 0 && action_handler != NULL);

        if (action.enabled != 0 && action_handler != NULL) {
            closure = (ViewModelPanelActionClosure *)calloc(
                1U, sizeof(*closure));
            if (closure != NULL) {
                closure->handler = action_handler;
                closure->user_data = user_data;
                (void)snprintf(
                    closure->action_id,
                    sizeof(closure->action_id),
                    "%s",
                    action.action_id);
                g_signal_connect_data(
                    button,
                    "clicked",
                    G_CALLBACK(on_action_clicked),
                    closure,
                    action_closure_destroy,
                    0);
            } else {
                gtk_widget_set_sensitive(button, FALSE);
            }
        }
        gtk_box_append(GTK_BOX(actions), button);
        appended += 1U;
    }

    if (appended > 0U) {
        gtk_box_append(GTK_BOX(container), actions);
    } else {
        release_unparented_widget(actions);
    }
    return appended;
}

/* Build a responsive scene from the selected market bar stored in the view model. */
static UmiStatus build_trading_chart_scene(
    const UmiUiViewModel *view,
    UmiChartRenderScene **out_scene)
{
    UmiUiValue has_bar;
    UmiChartCandle candle = {0};
    UmiChartPlotViewport viewport;
    UmiChartPlotStyle style;
    UmiChartRenderScene *scene = NULL;
    UmiStatus status;

    if (view == NULL || out_scene == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_scene = NULL;
    status = umi_ui_view_model_get_property(view, "trading.has-bar", &has_bar);
    if (status != UMI_STATUS_OK) return status;
    if (has_bar.kind != UMI_UI_VALUE_BOOLEAN || !has_bar.boolean_value) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = real_property(view, "trading.open", &candle.open);
    if (status == UMI_STATUS_OK)
        status = real_property(view, "trading.high", &candle.high);
    if (status == UMI_STATUS_OK)
        status = real_property(view, "trading.low", &candle.low);
    if (status == UMI_STATUS_OK)
        status = real_property(view, "trading.close", &candle.close);
    if (status == UMI_STATUS_OK)
        status = real_property(view, "trading.volume", &candle.volume);
    if (status != UMI_STATUS_OK) return status;

    candle.time_ms = 0;
    status = umi_chart_render_scene_create(32U, &scene);
    if (status != UMI_STATUS_OK) return status;
    status = umi_chart_render_scene_set_coordinate_size(scene, 640.0, 360.0);
    if (status == UMI_STATUS_OK) {
        status = umi_chart_plot_viewport_from_candles(
            &candle,
            1U,
            (UmiChartRenderRectangle){12.0, 12.0, 616.0, 336.0},
            0.08,
            &viewport);
    }
    umi_chart_plot_style_dark(&style);
    style.background_color.alpha = 0.0;
    if (status == UMI_STATUS_OK)
        status = umi_chart_plot_add_frame(scene, &viewport, &style);
    if (status == UMI_STATUS_OK) {
        status = umi_chart_plot_add_candlesticks(
            scene, &candle, 1U, &viewport, &style);
    }
    if (status != UMI_STATUS_OK) {
        umi_chart_render_scene_destroy(scene);
        return status;
    }
    *out_scene = scene;
    return UMI_STATUS_OK;
}

/* Build a Framework render scene from generic chart properties. */
static UmiStatus build_generic_chart_scene(
    const UmiUiViewModel *view,
    const char *chart_type,
    size_t point_count,
    UmiChartRenderScene **out_scene)
{
    UmiChartPoint *points;
    UmiChartRenderScene *scene = NULL;
    UmiChartPlotViewport viewport;
    UmiChartPlotStyle style;
    UmiStatus status;
    size_t index;
    double minimum_value = 0.0;
    double maximum_value = 0.0;

    if (view == NULL || chart_type == NULL || out_scene == NULL ||
        point_count == 0U || point_count > UMI_CHART_MAX_POINTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_scene = NULL;
    points = (UmiChartPoint *)calloc(point_count, sizeof(*points));
    if (points == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    for (index = 0U; index < point_count; ++index) {
        char key[UMI_UI_ID_CAPACITY];
        double value = 0.0;

        (void)snprintf(
            key, sizeof(key), "umicom.chart.point.%zu", index);
        status = real_property(view, key, &value);
        if (status != UMI_STATUS_OK) {
            free(points);
            return status;
        }
        points[index].time_ms = (int64_t)index;
        points[index].value = value;
        if (index == 0U || value < minimum_value) minimum_value = value;
        if (index == 0U || value > maximum_value) maximum_value = value;
    }

    if (minimum_value == maximum_value) {
        const double padding = minimum_value == 0.0
            ? 1.0
            : (minimum_value < 0.0 ? -minimum_value : minimum_value) * 0.10;
        minimum_value -= padding;
        maximum_value += padding;
    } else {
        const double padding = (maximum_value - minimum_value) * 0.08;
        minimum_value -= padding;
        maximum_value += padding;
    }

    status = umi_chart_render_scene_create(
        point_count * 4U + 128U, &scene);
    if (status == UMI_STATUS_OK)
        status = umi_chart_render_scene_set_coordinate_size(scene, 640.0, 260.0);
    viewport.area = (UmiChartRenderRectangle){12.0, 12.0, 616.0, 236.0};
    viewport.start_ms = 0;
    viewport.end_ms = point_count > 1U ? (int64_t)(point_count - 1U) : 1;
    viewport.minimum_value = minimum_value;
    viewport.maximum_value = maximum_value;
    umi_chart_plot_style_dark(&style);
    style.background_color.alpha = 0.0;
    if (status == UMI_STATUS_OK)
        status = umi_chart_plot_add_frame(scene, &viewport, &style);
    if (status == UMI_STATUS_OK && strcmp(chart_type, "bar") == 0) {
        status = umi_chart_plot_add_bar_series(
            scene, points, point_count, &viewport, &style);
    } else if (status == UMI_STATUS_OK) {
        status = umi_chart_plot_add_line_series(
            scene, points, point_count, &viewport, &style);
    }
    free(points);
    if (status != UMI_STATUS_OK) {
        umi_chart_render_scene_destroy(scene);
        return status;
    }
    *out_scene = scene;
    return UMI_STATUS_OK;
}

/* Create the shared chart renderer when a view exposes recognised chart data. */
static GtkWidget *create_chart(
    const UmiUiViewModel *view,
    const char *view_kind,
    const char *title)
{
    UmiUiViewSnapshot snapshot;
    UmiUiValue chart_type_value;
    UmiWsChartSurface chart = {0};
    UmiChartRenderScene *scene = NULL;
    GtkWidget *widget;
    UmiStatus status;

    if (view == NULL || view_kind == NULL || title == NULL) return NULL;
    if (umi_ui_view_model_snapshot(view, &snapshot) != UMI_STATUS_OK) {
        return NULL;
    }

    if (strcmp(view_kind, "trading-chart") == 0) {
        status = build_trading_chart_scene(view, &scene);
        /* Missing market data is a valid empty chart state. */
        if (status != UMI_STATUS_OK) {
            scene = NULL;
            status = UMI_STATUS_OK;
        }
    } else {
        int64_t point_count = 0;
        const char *chart_type = string_property(
            view, "umicom.chart.type", &chart_type_value, "");

        if (chart_type[0] == '\0' ||
            integer_property(
                view, "umicom.chart.point-count", &point_count) !=
                UMI_STATUS_OK ||
            point_count <= 0 ||
            (uint64_t)point_count > (uint64_t)UMI_CHART_MAX_POINTS) {
            return NULL;
        }
        status = build_generic_chart_scene(
            view, chart_type, (size_t)point_count, &scene);
    }
    if (status != UMI_STATUS_OK) return NULL;
    if (umi_ws_chart_surface_init(&chart, snapshot.view_id, title) !=
        UMI_STATUS_OK) {
        umi_chart_render_scene_destroy(scene);
        return NULL;
    }
    chart.show_grid = true;
    chart.sync_symbol = true;
    chart.sync_time = true;
    chart.sync_crosshair = true;
    widget = umi_gtk4_ws_chart_surface_create_with_scene(&chart, scene);
    umi_chart_render_scene_destroy(scene);
    if (widget != NULL) {
        gtk_widget_set_size_request(widget, -1, 220);
        gtk_widget_set_hexpand(widget, TRUE);
    }
    return widget;
}

/* Build a simple table from portable column, row and cell properties. */
static GtkWidget *create_table(const UmiUiViewModel *view)
{
    int64_t column_count_value;
    int64_t row_count_value;
    size_t column_count;
    size_t row_count;
    GtkWidget *scroller;
    GtkWidget *grid;
    size_t column;
    size_t row;

    if (view == NULL ||
        integer_property(
            view, "umicom.table.column-count", &column_count_value) !=
            UMI_STATUS_OK ||
        integer_property(
            view, "umicom.table.row-count", &row_count_value) !=
            UMI_STATUS_OK ||
        column_count_value <= 0 || row_count_value < 0 ||
        (uint64_t)column_count_value >
            (uint64_t)UMI_GTK4_VIEW_MODEL_TABLE_MAX_COLUMNS ||
        (uint64_t)row_count_value >
            (uint64_t)UMI_GTK4_VIEW_MODEL_TABLE_MAX_ROWS) {
        return NULL;
    }

    column_count = (size_t)column_count_value;
    row_count = (size_t)row_count_value;
    scroller = gtk_scrolled_window_new();
    grid = gtk_grid_new();
    if (scroller == NULL || grid == NULL) return scroller;
    gtk_widget_add_css_class(scroller, "umicom-view-model-table");
    gtk_widget_add_css_class(grid, "umicom-data-grid");
    gtk_grid_set_column_spacing(GTK_GRID(grid), 1);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 1);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroller),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), grid);

    for (column = 0U; column < column_count; ++column) {
        char key[UMI_UI_ID_CAPACITY];
        char fallback[32U];
        UmiUiValue value;
        const char *label_text;
        GtkWidget *label;

        (void)snprintf(
            key, sizeof(key), "umicom.table.column.%zu", column);
        (void)snprintf(fallback, sizeof(fallback), "Column %zu", column + 1U);
        label_text = string_property(view, key, &value, fallback);
        label = gtk_label_new(label_text);
        if (label == NULL) continue;
        gtk_widget_add_css_class(label, "umicom-data-grid-header");
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_grid_attach(
            GTK_GRID(grid), label, (int)column, 0, 1, 1);
    }

    if (row_count == 0U) {
        GtkWidget *empty = gtk_label_new("No rows");
        if (empty != NULL) {
            gtk_widget_add_css_class(empty, "dim-label");
            gtk_grid_attach(
                GTK_GRID(grid), empty, 0, 1, (int)column_count, 1);
        }
        return scroller;
    }

    for (row = 0U; row < row_count; ++row) {
        for (column = 0U; column < column_count; ++column) {
            char key[UMI_UI_ID_CAPACITY];
            char buffer[UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY];
            UmiUiValue value;
            GtkWidget *cell;

            (void)snprintf(
                key,
                sizeof(key),
                "umicom.table.cell.%zu.%zu",
                row,
                column);
            if (umi_ui_view_model_get_property(view, key, &value) ==
                UMI_STATUS_OK) {
                value_text(&value, buffer, sizeof(buffer));
            } else {
                buffer[0] = '\0';
            }
            cell = gtk_label_new(buffer);
            if (cell == NULL) continue;
            gtk_label_set_xalign(GTK_LABEL(cell), 0.0F);
            gtk_label_set_selectable(GTK_LABEL(cell), TRUE);
            gtk_widget_add_css_class(cell, "umicom-data-grid-cell");
            gtk_widget_set_hexpand(cell, TRUE);
            gtk_grid_attach(
                GTK_GRID(grid),
                cell,
                (int)column,
                (int)(row + 1U),
                1,
                1);
        }
    }
    return scroller;
}

/* Render the portable model while choosing specialised Framework components
 * only through reserved view-kind properties. */
GtkWidget *umi_gtk4_view_model_panel_create(
    UmiUiViewModel *view,
    UmiGtk4ViewModelActionHandler action_handler,
    void *user_data)
{
    GtkWidget *scroller;
    GtkWidget *content;
    GtkWidget *title;
    GtkWidget *summary;
    GtkWidget *status_card;
    GtkWidget *metrics;
    GtkWidget *rows;
    GtkWidget *technical_metrics;
    GtkWidget *technical_expander;
    GtkWidget *table;
    GtkWidget *chart;
    UmiUiPropertyBag *properties;
    UmiUiValue title_value;
    UmiUiValue summary_value;
    UmiUiValue kind_value;
    UmiUiViewSnapshot snapshot;
    const char *title_text;
    const char *summary_text;
    const char *view_kind;
    size_t index;
    size_t action_count;
    size_t ordinary_property_count = 0U;
    size_t technical_property_count = 0U;
    size_t row_property_count = 0U;
    int metric_row = 0;
    int technical_row = 0;

    if (view == NULL) return NULL;
    title_text = string_property(
        view, "title", &title_value, "Framework View");
    summary_text = string_property(
        view, "summary", &summary_value, "");
    view_kind = string_property(
        view, "umicom.view-kind", &kind_value, "generic");

    scroller = gtk_scrolled_window_new();
    content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    metrics = gtk_grid_new();
    rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    technical_metrics = gtk_grid_new();
    if (scroller == NULL || content == NULL || metrics == NULL ||
        rows == NULL || technical_metrics == NULL) {
        return NULL;
    }
    gtk_widget_add_css_class(scroller, "umicom-view-model-panel");
    gtk_widget_add_css_class(content, "umicom-view-model-panel-content");
    gtk_widget_add_css_class(metrics, "umicom-view-model-metrics");
    gtk_widget_add_css_class(rows, "umicom-view-model-rows");
    gtk_grid_set_column_spacing(GTK_GRID(metrics), 12);
    gtk_grid_set_row_spacing(GTK_GRID(metrics), 4);
    gtk_grid_set_column_spacing(GTK_GRID(technical_metrics), 12);
    gtk_grid_set_row_spacing(GTK_GRID(technical_metrics), 4);
    gtk_widget_set_hexpand(content, TRUE);
    gtk_widget_set_vexpand(content, TRUE);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroller),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), content);
    if (umi_ui_view_model_snapshot(view, &snapshot) == UMI_STATUS_OK) {
        (void)umi_gtk4_automation_tag_widget(scroller, snapshot.view_id);
    }

    title = gtk_label_new(title_text);
    summary = gtk_label_new(summary_text);
    if (title == NULL || summary == NULL) return scroller;
    gtk_widget_add_css_class(title, "title-3");
    gtk_widget_add_css_class(summary, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(summary), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(summary), TRUE);
    gtk_widget_set_visible(title, strcmp(view_kind, "product-panel") != 0);
    gtk_widget_set_visible(summary, summary_text[0] != '\0');
    gtk_box_append(GTK_BOX(content), title);
    gtk_box_append(GTK_BOX(content), summary);

    status_card = create_status_card(view);
    if (status_card != NULL) gtk_box_append(GTK_BOX(content), status_card);

    chart = create_chart(view, view_kind, title_text);
    table = create_table(view);
    if (chart != NULL) gtk_box_append(GTK_BOX(content), chart);
    if (table != NULL) gtk_box_append(GTK_BOX(content), table);

    properties = umi_ui_view_model_properties(view);
    if (properties != NULL) {
        for (index = 0U; index < umi_ui_property_bag_count(properties); ++index) {
            UmiUiPropertySnapshot property;

            if (umi_ui_property_bag_at(properties, index, &property) !=
                UMI_STATUS_OK) {
                continue;
            }
            if (is_technical_property(property.key)) {
                append_property_row(
                    technical_metrics,
                    technical_row++,
                    technical_property_label(property.key),
                    &property.value);
                technical_property_count += 1U;
                continue;
            }
            if (is_hidden_property(property.key) ||
                property_has_prefix(
                    property.key, UMI_GTK4_TABLE_PROPERTY_PREFIX) ||
                property_has_prefix(
                    property.key, UMI_GTK4_CHART_PROPERTY_PREFIX)) {
                continue;
            }
            if (is_row_property(property.key)) {
                char buffer[UMI_GTK4_VIEW_MODEL_TEXT_CAPACITY];
                GtkWidget *row;

                value_text(&property.value, buffer, sizeof(buffer));
                row = gtk_label_new(buffer);
                if (row != NULL) {
                    gtk_label_set_xalign(GTK_LABEL(row), 0.0F);
                    gtk_widget_add_css_class(row, "umicom-view-model-row");
                    gtk_box_append(GTK_BOX(rows), row);
                    row_property_count += 1U;
                }
                continue;
            }
            append_property_row(
                metrics, metric_row++, property.key, &property.value);
            ordinary_property_count += 1U;
        }
    }

    if (ordinary_property_count > 0U) {
        gtk_box_append(GTK_BOX(content), metrics);
    } else {
        release_unparented_widget(metrics);
    }
    if (row_property_count > 0U) {
        gtk_box_append(GTK_BOX(content), rows);
    } else {
        release_unparented_widget(rows);
    }

    action_count = append_actions(
        content, view, action_handler, user_data);

    if (technical_property_count > 0U) {
        technical_expander = gtk_expander_new("Technical details");
        if (technical_expander != NULL) {
            gtk_widget_add_css_class(
                technical_expander, "umicom-technical-details");
            gtk_expander_set_child(
                GTK_EXPANDER(technical_expander), technical_metrics);
            gtk_box_append(GTK_BOX(content), technical_expander);
        } else {
            release_unparented_widget(technical_metrics);
        }
    } else {
        release_unparented_widget(technical_metrics);
    }

    if (status_card == NULL && table == NULL && chart == NULL &&
        ordinary_property_count == 0U && row_property_count == 0U &&
        action_count == 0U) {
        GtkWidget *empty = create_empty_state(view);
        if (empty != NULL) gtk_box_append(GTK_BOX(content), empty);
    }
    return scroller;
}
