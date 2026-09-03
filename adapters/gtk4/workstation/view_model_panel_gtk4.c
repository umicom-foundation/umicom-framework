/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/view_model_panel_gtk4.c
 *
 * PURPOSE:
 *   Render toolkit-neutral Umicom view models as useful GTK4 workstation
 *   panels, including status, metrics, tables, charts, technical disclosure
 *   and command actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/view_model_panel.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/gtk4/workstation/chart_surface.h"
#include "umicom/ui/gtk4/workstation/table_surface.h"
#include "umicom/ui/gtk4/workstation/trading_chart.h"

#define UMI_GTK4_TECHNICAL_PROPERTY_PREFIX "umicom.technical."

typedef struct ViewModelPanelActionClosure {
    UmiGtk4ViewModelPanelActionHandler handler;
    void *user_data;
    char action_id[UMI_UI_ID_CAPACITY];
} ViewModelPanelActionClosure;

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

/* Release one action closure through GTK's required two-argument notifier
 * signature, keeping signal ownership explicit and type-safe. */
static void action_closure_destroy(gpointer data, GClosure *closure)
{
    (void)closure;
    g_free(data);
}

/* Convert the small portable value union to readable text. */
static const char *value_text(
    const UmiUiValue *value,
    char *buffer,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || buffer == NULL || capacity == 0U) return "";
    /* Select the behaviour associated with the requested command or state value. */
    switch (value->type) {
    case UMI_UI_VALUE_BOOLEAN:
        return value->as.boolean ? "Yes" : "No";
    case UMI_UI_VALUE_INTEGER:
        (void)snprintf(
            buffer, capacity, "%lld",
            (long long)value->as.integer);
        return buffer;
    case UMI_UI_VALUE_NUMBER:
        (void)snprintf(buffer, capacity, "%.8g", value->as.number);
        return buffer;
    case UMI_UI_VALUE_STRING:
        return value->as.text;
    case UMI_UI_VALUE_EMPTY:
    default:
        return "";
    }
}

/* Internal fields are consumed by the specialised renderers and should not
 * appear again as user-facing metric rows. */
static bool is_hidden_property(const char *key)
{
    static const char *const hidden[] = {
        "title",
        "summary",
        "umicom.view-kind",
        "State",
        "Message",
        "Badge",
        "Progress percent",
        UMI_UI_COMMAND_VIEW_PROPERTY_ACTION_COUNT,
        UMI_UI_COMMAND_VIEW_PROPERTY_STATUS_TEXT
    };
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key == NULL) return true;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(hidden) / sizeof(hidden[0]); ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(key, hidden[index]) == 0) return true;
    }
    return strncmp(
        key,
        UMI_UI_COMMAND_VIEW_RESERVED_PREFIX,
        strlen(UMI_UI_COMMAND_VIEW_RESERVED_PREFIX)) == 0;
}

/* Technical properties remain available but do not compete with ordinary
 * product content. */
static bool is_technical_property(const char *key)
{
    const size_t prefix_length =
        strlen(UMI_GTK4_TECHNICAL_PROPERTY_PREFIX);

    return key != NULL &&
           strncmp(key,
                   UMI_GTK4_TECHNICAL_PROPERTY_PREFIX,
                   prefix_length) == 0;
}

/* Remove the storage namespace before presenting a technical property label. */
static const char *technical_property_label(const char *key)
{
    return is_technical_property(key)
        ? key + strlen(UMI_GTK4_TECHNICAL_PROPERTY_PREFIX)
        : key;
}

/* Return an optional string property without inventing a new ownership rule. */
static const char *string_property(
    const UmiUiViewModel *view,
    const char *key)
{
    const UmiUiValue *value =
        umi_ui_view_model_get_property(view, key);

    return value != NULL && value->type == UMI_UI_VALUE_STRING
        ? value->as.text
        : "";
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
    char buffer[96U];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (grid == NULL || label_text == NULL || value == NULL) return;
    label = gtk_label_new(label_text);
    content = gtk_label_new(value_text(value, buffer, sizeof(buffer)));
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
    const UmiUiValue *state_value;
    const UmiUiValue *message_value;
    const UmiUiValue *badge_value;
    const UmiUiValue *progress_value;
    GtkWidget *card;
    GtkWidget *heading;
    GtkWidget *badge;
    GtkWidget *state;
    GtkWidget *message;
    GtkWidget *progress;
    char state_buffer[96U];
    char message_buffer[256U];
    char badge_buffer[96U];
    char progress_text[32U];
    const char *state_text;
    const char *message_text;
    const char *badge_text;
    int64_t percent;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return NULL;
    state_value = umi_ui_view_model_get_property(view, "State");
    message_value = umi_ui_view_model_get_property(view, "Message");
    badge_value = umi_ui_view_model_get_property(view, "Badge");
    progress_value = umi_ui_view_model_get_property(
        view, "Progress percent");
    if (state_value == NULL && message_value == NULL &&
        badge_value == NULL && progress_value == NULL) {
        return NULL;
    }

    state_text = value_text(
        state_value, state_buffer, sizeof(state_buffer));
    message_text = value_text(
        message_value, message_buffer, sizeof(message_buffer));
    badge_text = value_text(
        badge_value, badge_buffer, sizeof(badge_buffer));

    card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    heading = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    state = gtk_label_new(state_text);
    badge = gtk_label_new(badge_text);
    message = gtk_label_new(message_text);
    progress = gtk_progress_bar_new();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    gtk_widget_set_visible(badge, badge_text[0] != '\0');
    gtk_widget_set_visible(message, message_text[0] != '\0');
    gtk_box_append(GTK_BOX(heading), state);
    gtk_box_append(GTK_BOX(heading), badge);
    gtk_box_append(GTK_BOX(card), heading);
    gtk_box_append(GTK_BOX(card), message);

    percent = progress_value != NULL &&
              progress_value->type == UMI_UI_VALUE_INTEGER
        ? progress_value->as.integer
        : 0;
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    (void)snprintf(
        progress_text, sizeof(progress_text), "%lld%%",
        (long long)percent);
    gtk_progress_bar_set_fraction(
        GTK_PROGRESS_BAR(progress), (double)percent / 100.0);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress), TRUE);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), progress_text);
    gtk_widget_set_visible(progress, progress_value != NULL);
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

/* Render every command through the stable action callback. */
static size_t append_actions(
    GtkWidget *container,
    const UmiUiViewModel *view,
    UmiGtk4ViewModelPanelActionHandler action_handler,
    void *user_data)
{
    GtkWidget *actions;
    size_t index;
    size_t appended = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (container == NULL || view == NULL) return 0U;
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_UI_COMMAND_VIEW_MAX_ACTIONS; ++index) {
        UmiUiCommandViewAction action;
        ViewModelPanelActionClosure *closure;
        GtkWidget *button;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ui_command_view_action(view, index, &action) !=
            UMI_STATUS_OK) {
            continue;
        }
        button = gtk_button_new_with_label(action.label);
        gtk_widget_set_sensitive(button, action.enabled);
        gtk_widget_set_tooltip_text(
            button,
            action.tooltip[0] != '\0' ? action.tooltip : action.label);
        closure = g_new0(ViewModelPanelActionClosure, 1);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (closure == NULL) continue;
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
        gtk_box_append(GTK_BOX(actions), button);
        appended += 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (appended > 0U) {
        gtk_box_append(GTK_BOX(container), actions);
    } else {
        g_object_ref_sink(actions);
        g_object_unref(actions);
    }
    return appended;
}

/* Build a simple table model from portable column and row properties. */
static GtkWidget *create_table(const UmiUiViewModel *view)
{
    const UmiUiValue *column_count_value;
    const UmiUiValue *row_count_value;
    UmiGtk4WorkstationTableModel table;
    size_t column_count;
    size_t row_count;
    size_t column;
    size_t row;

    column_count_value = umi_ui_view_model_get_property(
        view, "umicom.table.column-count");
    row_count_value = umi_ui_view_model_get_property(
        view, "umicom.table.row-count");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (column_count_value == NULL ||
        column_count_value->type != UMI_UI_VALUE_INTEGER ||
        row_count_value == NULL ||
        row_count_value->type != UMI_UI_VALUE_INTEGER ||
        column_count_value->as.integer <= 0 ||
        row_count_value->as.integer < 0) {
        return NULL;
    }
    column_count = (size_t)column_count_value->as.integer;
    row_count = (size_t)row_count_value->as.integer;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (column_count > UMI_GTK4_WORKSTATION_TABLE_MAX_COLUMNS ||
        row_count > UMI_GTK4_WORKSTATION_TABLE_MAX_ROWS) {
        return NULL;
    }
    (void)memset(&table, 0, sizeof(table));
    table.column_count = column_count;
    table.row_count = row_count;

    /* Visit each bounded item once so every record receives the same rule. */
    for (column = 0U; column < column_count; ++column) {
        char key[UMI_UI_ID_CAPACITY];
        const UmiUiValue *value;

        (void)snprintf(
            key, sizeof(key), "umicom.table.column.%zu", column);
        value = umi_ui_view_model_get_property(view, key);
        (void)snprintf(
            table.columns[column],
            sizeof(table.columns[column]),
            "%s",
            value != NULL && value->type == UMI_UI_VALUE_STRING
                ? value->as.text
                : "");
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (row = 0U; row < row_count; ++row) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (column = 0U; column < column_count; ++column) {
            char key[UMI_UI_ID_CAPACITY];
            char buffer[96U];
            const UmiUiValue *value;

            (void)snprintf(
                key,
                sizeof(key),
                "umicom.table.cell.%zu.%zu",
                row,
                column);
            value = umi_ui_view_model_get_property(view, key);
            (void)snprintf(
                table.cells[row][column],
                sizeof(table.cells[row][column]),
                "%s",
                value_text(value, buffer, sizeof(buffer)));
        }
    }
    return umi_gtk4_ws_table_surface_create(&table);
}

/* Create the shared chart renderer when a view exposes chart properties. */
static GtkWidget *create_chart(const UmiUiViewModel *view)
{
    const UmiUiValue *chart_type =
        umi_ui_view_model_get_property(view, "umicom.chart.type");
    const UmiUiValue *point_count_value =
        umi_ui_view_model_get_property(view, "umicom.chart.point-count");
    size_t point_count;
    UmiGtk4WorkstationChartModel chart;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chart_type == NULL || chart_type->type != UMI_UI_VALUE_STRING ||
        point_count_value == NULL ||
        point_count_value->type != UMI_UI_VALUE_INTEGER ||
        point_count_value->as.integer <= 0) {
        return NULL;
    }
    point_count = (size_t)point_count_value->as.integer;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (point_count > UMI_GTK4_WORKSTATION_CHART_MAX_POINTS) {
        return NULL;
    }
    (void)memset(&chart, 0, sizeof(chart));
    chart.point_count = point_count;
    chart.type = strcmp(chart_type->as.text, "bar") == 0
        ? UMI_GTK4_WORKSTATION_CHART_BAR
        : UMI_GTK4_WORKSTATION_CHART_LINE;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < point_count; ++index) {
        char key[UMI_UI_ID_CAPACITY];
        const UmiUiValue *value;

        (void)snprintf(
            key, sizeof(key), "umicom.chart.point.%zu", index);
        value = umi_ui_view_model_get_property(view, key);
        chart.values[index] =
            value != NULL && value->type == UMI_UI_VALUE_NUMBER
                ? value->as.number
                : value != NULL && value->type == UMI_UI_VALUE_INTEGER
                    ? (double)value->as.integer
                    : 0.0;
    }
    return umi_gtk4_ws_chart_surface_create(&chart);
}

/* Render the portable model while choosing specialised Framework components
 * only through reserved view-kind properties. */
GtkWidget *umi_gtk4_view_model_panel_create(
    const UmiUiViewModel *view,
    UmiGtk4ViewModelPanelActionHandler action_handler,
    void *user_data)
{
    GtkWidget *scroller;
    GtkWidget *content;
    GtkWidget *title;
    GtkWidget *summary;
    GtkWidget *status_card;
    GtkWidget *metrics;
    GtkWidget *technical_metrics;
    GtkWidget *technical_expander;
    GtkWidget *table;
    GtkWidget *chart;
    const char *title_text;
    const char *summary_text;
    const char *view_kind;
    size_t index;
    size_t action_count;
    size_t ordinary_property_count = 0U;
    size_t technical_property_count = 0U;
    int metric_row = 0;
    int technical_row = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return NULL;
    scroller = gtk_scrolled_window_new();
    content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    metrics = gtk_grid_new();
    technical_metrics = gtk_grid_new();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (scroller == NULL || content == NULL || metrics == NULL ||
        technical_metrics == NULL) {
        return NULL;
    }
    gtk_widget_add_css_class(scroller, "umicom-view-model-panel");
    gtk_widget_add_css_class(content, "umicom-view-model-panel-content");
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroller),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(scroller), content);
    gtk_grid_set_column_spacing(GTK_GRID(metrics), 12);
    gtk_grid_set_row_spacing(GTK_GRID(metrics), 4);
    gtk_grid_set_column_spacing(GTK_GRID(technical_metrics), 12);
    gtk_grid_set_row_spacing(GTK_GRID(technical_metrics), 4);

    title_text = string_property(view, "title");
    summary_text = string_property(view, "summary");
    view_kind = string_property(view, "umicom.view-kind");
    title = gtk_label_new(
        title_text[0] != '\0' ? title_text : view->view_id);
    summary = gtk_label_new(summary_text);
    gtk_widget_add_css_class(title, "title-3");
    gtk_widget_add_css_class(summary, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(summary), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(summary), TRUE);
    /* Product panels already have a Framework panel-frame title. Keep the
     * content heading for standalone views while avoiding duplicate headings
     * inside normal product workspaces. */
    gtk_widget_set_visible(
        title, strcmp(view_kind, "product-panel") != 0);
    gtk_widget_set_visible(summary, summary_text[0] != '\0');
    gtk_box_append(GTK_BOX(content), title);
    gtk_box_append(GTK_BOX(content), summary);

    status_card = create_status_card(view);
    /* Apply this branch only when its contract condition is satisfied. */
    if (status_card != NULL) {
        gtk_box_append(GTK_BOX(content), status_card);
    }

    table = create_table(view);
    chart = create_chart(view);
    /* Apply this branch only when its contract condition is satisfied. */
    if (chart != NULL) {
        gtk_box_append(GTK_BOX(content), chart);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (table != NULL) {
        gtk_box_append(GTK_BOX(content), table);
    }

    /* Keep ordinary product information visible and route engineering facts
     * to a collapsed disclosure without losing any evidence. */
    for (index = 0U; index < view->property_count; ++index) {
        const UmiUiProperty *property = &view->properties[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (is_technical_property(property->key)) {
            append_property_row(
                technical_metrics,
                technical_row++,
                technical_property_label(property->key),
                &property->value);
            technical_property_count += 1U;
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (is_hidden_property(property->key) ||
            strncmp(property->key, "umicom.table.",
                    strlen("umicom.table.")) == 0 ||
            strncmp(property->key, "umicom.chart.",
                    strlen("umicom.chart.")) == 0) {
            continue;
        }
        append_property_row(
            metrics, metric_row++, property->key, &property->value);
        ordinary_property_count += 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (ordinary_property_count > 0U) {
        gtk_box_append(GTK_BOX(content), metrics);
    } else {
        g_object_ref_sink(metrics);
        g_object_unref(metrics);
    }

    action_count = append_actions(
        content, view, action_handler, user_data);

    /* Technical evidence is always available on demand but collapsed by
     * default so product users see tasks and outcomes first. */
    if (technical_property_count > 0U) {
        technical_expander = gtk_expander_new("Technical details");
        gtk_widget_add_css_class(
            technical_expander, "umicom-technical-details");
        gtk_expander_set_child(
            GTK_EXPANDER(technical_expander), technical_metrics);
        gtk_box_append(GTK_BOX(content), technical_expander);
    } else {
        g_object_ref_sink(technical_metrics);
        g_object_unref(technical_metrics);
    }

    /* A true empty state uses space intentionally instead of showing a large
     * blank panel with no explanation. */
    if (status_card == NULL && table == NULL && chart == NULL &&
        ordinary_property_count == 0U && action_count == 0U) {
        gtk_box_append(GTK_BOX(content), create_empty_state(view));
    }
    return scroller;
}
