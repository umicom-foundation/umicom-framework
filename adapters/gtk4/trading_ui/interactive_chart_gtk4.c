/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/interactive_chart_gtk4.c
 * PURPOSE: Connect chart navigation, drawing tools and reviewed order drafting to native input.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading_ui/gtk4/interactive_chart.h"
#include "umicom/trading_ui/chart_scene.h"
#include "umicom/chart/adapters/cairo_renderer.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/drop_down.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHART_STATE "umicom-interactive-chart-state"
typedef struct InteractiveChart {
    int references;
    UmiGtk4TradingPanelContext *context;
    GtkWidget *root, *area, *status, *message, *objects, *period, *studies;
    UmiTradingChartSceneInfo info;
    UmiChartRenderScene *scene;
    UmiChartPoint first_point;
    char drawing_ids[UMI_CHART_DRAWING_CAPACITY][128];
    uint64_t drawing_versions[UMI_CHART_DRAWING_CAPACITY];
    size_t drawing_count;
    uint64_t drawing_revision;
    char objects_instrument[UMI_FINANCE_ID_CAPACITY];
    guint timer;
    int tool, have_first, have_pointer, dragging;
    double pointer_x, pointer_y;
} InteractiveChart;

static InteractiveChart *ChartState(GtkWidget *root)
{ return g_object_get_data(G_OBJECT(root), CHART_STATE); }
static void ChartRelease(gpointer data)
{
    InteractiveChart *state = data;
    if (--state->references != 0) return;
    umi_chart_render_scene_destroy(state->scene);
    g_free(state);
}
static void ChartRootDestroyed(gpointer data)
{
    InteractiveChart *state = data;
    if (state->timer != 0U) g_source_remove(state->timer);
    state->timer = 0U;
    state->root = NULL;
    /* An externally retained drawing area owns one remaining read-only scene
     * reference. Its draw callback never touches the destroyed product model. */
    state->context = NULL;
    ChartRelease(state);
}
static void ChartMessage(InteractiveChart *state, const char *message)
{ gtk_label_set_text(GTK_LABEL(state->message), message); }

static void ChartObjectsRefresh(InteractiveChart *state)
{
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(
        umi_trading_workspace_charts(state->context->workspace));
    uint64_t revision = umi_chart_drawing_registry_revision(registry);
    if (revision == state->drawing_revision && strcmp(state->objects_instrument, state->info.instrument_id) == 0) return;
    char selected[128] = {0};
    guint prior = gtk_drop_down_get_selected(GTK_DROP_DOWN(state->objects));
    if (prior < state->drawing_count) (void)snprintf(selected, sizeof selected, "%s", state->drawing_ids[prior]);
    GtkStringList *list = gtk_string_list_new(NULL);
    guint next = GTK_INVALID_LIST_POSITION;
    state->drawing_count = 0;
    for (size_t i = 0; i < umi_chart_drawing_registry_count(registry); ++i) {
        UmiChartDrawingSnapshot drawing;
        if (umi_chart_drawing_registry_at(registry, i, &drawing) != UMI_STATUS_OK ||
            strcmp(drawing.pane_id, state->info.instrument_id) != 0) continue;
        if (strcmp(drawing.tool, "trend") != 0 && strcmp(drawing.tool, "support") != 0 &&
            strcmp(drawing.tool, "resistance") != 0) continue;
        size_t index = state->drawing_count++;
        (void)snprintf(state->drawing_ids[index], sizeof state->drawing_ids[index], "%s", drawing.id);
        state->drawing_versions[index] = drawing.revision;
        char label[196];
        (void)snprintf(label, sizeof label, "%s %.8g | %.96s%s", drawing.tool, drawing.value1,
            drawing.id, drawing.locked ? " (locked)" : "");
        char *display = g_utf8_make_valid(label, -1);
        gtk_string_list_append(list, display);
        g_free(display);
        if (strcmp(selected, drawing.id) == 0) next = (guint)index;
    }
    gtk_drop_down_set_model(GTK_DROP_DOWN(state->objects), G_LIST_MODEL(list));
    g_object_unref(list);
    if (next == GTK_INVALID_LIST_POSITION && state->drawing_count > 0) next = (guint)(state->drawing_count - 1U);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(state->objects), next);
    state->drawing_revision = revision;
    (void)snprintf(state->objects_instrument, sizeof state->objects_instrument, "%s", state->info.instrument_id);
}
static void ChartRefresh(InteractiveChart *state)
{
    if (state == NULL || state->context == NULL) return;
    UmiChartRenderScene *scene = NULL;
    UmiTradingChartSceneInfo info = {0};
    UmiStatus status = UmiTradingChartBuildScene(state->context->workspace, &info, &scene);
    if (status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND) {
        if (strcmp(state->info.instrument_id, info.instrument_id) != 0) {
            state->have_first = 0;
        }
        umi_chart_render_scene_destroy(state->scene);
        state->scene = scene;
        state->info = info;
        char text[192];
        (void)snprintf(text, sizeof text, "%s | %zu of %zu retained bars | wheel: zoom | drag: pan | UTC",
            info.instrument_id[0] ? info.instrument_id : "Waiting for retained market history",
            info.window.count, info.retained_bars);
        gtk_label_set_text(GTK_LABEL(state->status), text);
        ChartObjectsRefresh(state);
    } else {
        /* Never allow a gesture to use an older price mapping after a failure. */
        umi_chart_render_scene_destroy(state->scene); state->scene = NULL;
        state->have_first = 0;
        ChartMessage(state, "Chart unavailable. Wait for valid market history.");
    }
    gtk_widget_queue_draw(state->area);
}
static gboolean ChartTick(gpointer data)
{ ChartRefresh(data); return G_SOURCE_CONTINUE; }

static void ChartDraw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data)
{
    InteractiveChart *state = data;
    (void)area;
    if (width <= 0 || height <= 0) return;
    cairo_set_source_rgb(cr, 0.045, 0.055, 0.075); cairo_paint(cr);
    if (state->scene == NULL) return;
    UmiChartCairoRendererContext context;
    UmiChartRenderer renderer;
    if (umi_chart_cairo_renderer_init(&context, cr, &renderer) == UMI_STATUS_OK)
        (void)umi_chart_renderer_render_scene(&renderer, state->scene, width, height);
    if (!state->have_pointer) return;
    UmiChartPoint point;
    if (UmiChartPlotUnmap(&state->info.price, state->pointer_x, state->pointer_y, &point) != UMI_STATUS_OK) return;
    cairo_save(cr);
    cairo_scale(cr, (double)width / UMI_TRADING_CHART_WIDTH, (double)height / UMI_TRADING_CHART_HEIGHT);
    cairo_rectangle(cr, state->info.price.area.x, state->info.price.area.y,
        state->info.price.area.width, state->info.price.area.height);
    cairo_clip(cr);
    double dash[] = {4, 4};
    cairo_set_dash(cr, dash, 2, 0); cairo_set_line_width(cr, 1);
    cairo_set_source_rgba(cr, 0.72, 0.77, 0.84, 0.75);
    cairo_move_to(cr, state->pointer_x, 22); cairo_line_to(cr, state->pointer_x, 410);
    cairo_move_to(cr, 18, state->pointer_y); cairo_line_to(cr, 894, state->pointer_y); cairo_stroke(cr);
    cairo_set_dash(cr, NULL, 0, 0);
    if (state->have_first) {
        double x, y;
        if (umi_chart_plot_map_time(&state->info.price, state->first_point.time_ms, &x) == UMI_STATUS_OK &&
            umi_chart_plot_map_value(&state->info.price, state->first_point.value, &y) == UMI_STATUS_OK) {
            cairo_set_source_rgb(cr, 0.98, 0.72, 0.2);
            cairo_move_to(cr, x, y); cairo_line_to(cr, state->pointer_x, state->pointer_y); cairo_stroke(cr);
        }
    }
    char label[96];
    (void)snprintf(label, sizeof label, "%.8g | %02d:%02d UTC", point.value,
        (int)((point.time_ms / 3600000) % 24), (int)((point.time_ms / 60000) % 60));
    cairo_set_source_rgb(cr, 0.9, 0.93, 0.98); cairo_set_font_size(cr, 12);
    cairo_move_to(cr, 25, 38); cairo_show_text(cr, label);
    cairo_restore(cr);
}
static UmiStatus ChartNavigate(InteractiveChart *state, int zoom, int pan, int fit)
{
    UmiChartNavigation navigation;
    UmiStatus status = UmiTradingWorkspaceGetChartNavigation(state->context->workspace, state->info.instrument_id, &navigation);
    if (status != UMI_STATUS_OK) return status;
    if (fit) navigation = (UmiChartNavigation){0};
    else {
        UmiChartCandle candles[UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY];
        size_t count = umi_trading_workspace_selected_bar_count(state->context->workspace);
        if (count == 0 || count > UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY) return UMI_STATUS_NOT_FOUND;
        for (size_t i = 0; i < count; ++i) {
            UmiBar bar;
            status = umi_trading_workspace_selected_bar_at(state->context->workspace, i, &bar);
            if (status != UMI_STATUS_OK) return status;
            candles[i] = (UmiChartCandle){bar.start_time_ms, bar.open, bar.high, bar.low, bar.close, bar.volume};
        }
        if (zoom != 0) status = UmiChartNavigationZoom(&navigation, candles, count, zoom);
        else status = UmiChartNavigationPan(&navigation, candles, count, pan);
        if (status != UMI_STATUS_OK) return status;
    }
    status = UmiTradingWorkspaceSetChartNavigation(state->context->workspace, state->info.instrument_id, &navigation);
    if (status == UMI_STATUS_OK) ChartRefresh(state);
    return status;
}
static void ChartNavigateClicked(GtkButton *button, gpointer root)
{
    InteractiveChart *state = ChartState(root);
    int action = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "chart-action"));
    UmiStatus status = ChartNavigate(state, action == 1 ? 1 : action == 2 ? -1 : 0,
        action == 3 ? -10 : action == 4 ? 10 : 0, action == 5);
    if (status != UMI_STATUS_OK) ChartMessage(state, "Navigation needs retained history for this instrument.");
}
static gboolean ChartScroll(GtkEventControllerScroll *controller, double dx, double dy, gpointer root)
{
    (void)controller; (void)dx;
    InteractiveChart *state = ChartState(root);
    if (dy != 0 && isfinite(dy)) (void)ChartNavigate(state, dy < 0 ? 1 : -1, 0, 0);
    return TRUE;
}
static void ChartMotion(GtkEventControllerMotion *controller, double x, double y, gpointer root)
{
    (void)controller;
    InteractiveChart *state = ChartState(root);
    int width = gtk_widget_get_width(state->area), height = gtk_widget_get_height(state->area);
    if (width <= 0 || height <= 0) return;
    state->pointer_x = x * UMI_TRADING_CHART_WIDTH / width;
    state->pointer_y = y * UMI_TRADING_CHART_HEIGHT / height;
    state->have_pointer = 1;
    gtk_widget_queue_draw(state->area);
}
static void ChartAreaLeave(GtkEventControllerMotion *controller, gpointer root)
{
    (void)controller;
    InteractiveChart *state = ChartState(root);
    state->have_pointer = 0; gtk_widget_queue_draw(state->area);
}
static void ChartToolChanged(GtkToggleButton *button, gpointer root)
{
    if (!gtk_toggle_button_get_active(button)) return;
    InteractiveChart *state = ChartState(root);
    state->tool = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "chart-tool"));
    state->have_first = 0;
    static const char *messages[] = {"Drag to pan; use the wheel or +/- buttons to zoom.",
        "Click two price points to draw a trend line.", "Click a price to mark support.",
        "Click a price to mark resistance.", "Click a price to prepare a Buy limit ticket.",
        "Click a price to prepare a Sell limit ticket."};
    ChartMessage(state, messages[state->tool]);
    gtk_widget_queue_draw(state->area);
}
static void ChartClick(GtkGestureClick *gesture, int presses, double x, double y, gpointer root)
{
    (void)gesture;
    InteractiveChart *state = ChartState(root);
    if (presses != 1 || state->scene == NULL || state->tool == 0) return;
    int width = gtk_widget_get_width(state->area), height = gtk_widget_get_height(state->area);
    if (width <= 0 || height <= 0) return;
    UmiChartPoint point;
    if (UmiChartPlotUnmap(&state->info.price, x * UMI_TRADING_CHART_WIDTH / width,
        y * UMI_TRADING_CHART_HEIGHT / height, &point) != UMI_STATUS_OK) return;
    if (state->tool >= 4) {
        if (state->context->controller == NULL) {
            ChartMessage(state, "Order entry is unavailable in this chart view.");
            return;
        }
        char instrument[UMI_FINANCE_ID_CAPACITY];
        (void)snprintf(instrument, sizeof instrument, "%s", state->info.instrument_id);
        UmiTradingUiController *controller = state->context->controller;
        UmiSide side = state->tool == 4 ? UMI_SIDE_BUY : UMI_SIDE_SELL;
        /* Dispatch publishes the complete draft to the existing order ticket.
         * A custom host may replace the panel; do not touch state afterward. */
        (void)UmiTradingUiControllerPrepareChartLimit(controller, instrument, side, point.value);
        return;
    }
    if (state->tool == 1 && !state->have_first) {
        state->first_point = point; state->have_first = 1;
            ChartMessage(state, "Choose the second point; Escape cancels this unfinished line.");
        gtk_widget_grab_focus(state->area); return;
    }
    const char *tool = state->tool == 1 ? "trend" : state->tool == 2 ? "support" : "resistance";
    UmiStatus status = UmiTradingWorkspaceAddChartDrawing(state->context->workspace, state->info.instrument_id,
        tool, state->tool == 1 ? state->first_point : point, point);
    if (status == UMI_STATUS_OK) { state->have_first = 0; ChartMessage(state, "Drawing added to this instrument for the current workspace session."); ChartRefresh(state); }
    else ChartMessage(state, "Drawing unchanged. Use distinct trend times and the current instrument.");
}
static void ChartDragBegin(GtkGestureDrag *gesture, double x, double y, gpointer root)
{
    (void)gesture; (void)x; (void)y;
    InteractiveChart *state = ChartState(root);
    state->dragging = state->tool == 0;
}
static void ChartDragEnd(GtkGestureDrag *gesture, double x, double y, gpointer root)
{
    (void)gesture; (void)y;
    InteractiveChart *state = ChartState(root);
    int width = gtk_widget_get_width(state->area);
    if (state->dragging && width > 0 && isfinite(x)) {
        double bars = -x / width * (double)state->info.window.count;
        bars = fmax(-(double)UMI_CHART_MAX_POINTS, fmin((double)UMI_CHART_MAX_POINTS, bars));
        (void)ChartNavigate(state, 0, (int)bars, 0);
    }
    state->dragging = 0;
}
static gboolean ChartKey(GtkEventControllerKey *controller, guint key, guint code, GdkModifierType mods, gpointer root)
{
    (void)controller; (void)code; (void)mods;
    InteractiveChart *state = ChartState(root);
    if (key == GDK_KEY_Escape) { state->have_first = 0; ChartMessage(state, "Unfinished drawing cancelled."); gtk_widget_queue_draw(state->area); return TRUE; }
    if (key == GDK_KEY_plus || key == GDK_KEY_equal || key == GDK_KEY_KP_Add) { (void)ChartNavigate(state, 1, 0, 0); return TRUE; }
    if (key == GDK_KEY_minus || key == GDK_KEY_KP_Subtract) { (void)ChartNavigate(state, -1, 0, 0); return TRUE; }
    if (key == GDK_KEY_Left || key == GDK_KEY_Right) { (void)ChartNavigate(state, 0, key == GDK_KEY_Left ? -10 : 10, 0); return TRUE; }
    if (key == GDK_KEY_Home) { (void)ChartNavigate(state, 0, 0, 1); return TRUE; }
    return FALSE;
}
static void ChartRemove(GtkButton *button, gpointer root)
{
    (void)button;
    InteractiveChart *state = ChartState(root);
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(state->objects));
    if (selected >= state->drawing_count) return;
    UmiStatus status = UmiTradingWorkspaceRemoveChartDrawing(state->context->workspace, state->info.instrument_id,
        state->drawing_ids[selected], state->drawing_versions[selected]);
    ChartMessage(state, status == UMI_STATUS_OK ? "Selected drawing removed." : "Drawing changed, is locked, or belongs to another instrument. Select it again.");
    ChartRefresh(state);
}
static void ChartStudyApply(InteractiveChart *state)
{
    UmiChartNavigation guard;
    if (UmiTradingWorkspaceGetChartNavigation(state->context->workspace, state->info.instrument_id, &guard) != UMI_STATUS_OK) return;
    if (umi_trading_workspace_set_chart_study(state->context->workspace,
        (UmiTradingChartStudy)gtk_drop_down_get_selected(GTK_DROP_DOWN(state->studies)),
        (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(state->period))) == UMI_STATUS_OK) ChartRefresh(state);
}
static void ChartStudyChanged(GObject *object, GParamSpec *spec, gpointer root)
{ (void)object; (void)spec; ChartStudyApply(ChartState(root)); }
static void ChartPeriodChanged(GtkSpinButton *button, gpointer root)
{ (void)button; ChartStudyApply(ChartState(root)); }
static GtkWidget *ChartButton(GtkWidget *box, const char *label, const char *tag,
    GCallback callback, GtkWidget *root)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    (void)umi_gtk4_automation_tag_widget(button, tag);
    gtk_box_append(GTK_BOX(box), button);
    g_signal_connect_object(button, "clicked", callback, G_OBJECT(root), 0);
    return button;
}
GtkWidget *UmiGtk4TradingInteractiveChartCreate(UmiGtk4TradingPanelContext *context)
{
    if (context == NULL || context->workspace == NULL) return NULL;
    UmiTradingWorkspaceSnapshot snapshot;
    if (umi_trading_workspace_snapshot(context->workspace, &snapshot) != UMI_STATUS_OK) return NULL;
    InteractiveChart *state = g_try_new0(InteractiveChart, 1);
    if (state == NULL) return NULL;
    state->references = 1; state->context = context;
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    state->root = root;
    g_object_set_data_full(G_OBJECT(root), CHART_STATE, state, ChartRootDestroyed);
    (void)umi_gtk4_automation_tag_widget(root, "trading.chart.panel");
    GtkWidget *top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_append(GTK_BOX(root), top);
    const char *labels[] = {"+", "-", "Earlier", "Later", "Fit / Latest"};
    const char *tags[] = {"trading.chart.zoom-in", "trading.chart.zoom-out", "trading.chart.earlier", "trading.chart.later", "trading.chart.fit"};
    for (int i = 0; i < 5; ++i) {
        GtkWidget *button = ChartButton(top, labels[i], tags[i], G_CALLBACK(ChartNavigateClicked), root);
        g_object_set_data(G_OBJECT(button), "chart-action", GINT_TO_POINTER(i + 1));
    }
    const char *studies[] = {"Candles", "SMA", "EMA", NULL};
    state->studies = gtk_drop_down_new_from_strings(studies);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(state->studies), (guint)snapshot.chart_study);
    state->period = gtk_spin_button_new_with_range(2, 200, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(state->period), (double)snapshot.chart_study_period);
    gtk_widget_set_tooltip_text(state->period, "Moving average period in retained bars");
    gtk_box_append(GTK_BOX(top), state->studies); gtk_box_append(GTK_BOX(top), state->period);
    (void)umi_gtk4_automation_tag_widget(state->studies, "trading.chart.study");
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *rail = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_append(GTK_BOX(root), body); gtk_box_append(GTK_BOX(body), rail);
    const char *tools[] = {"Cursor", "Trend", "Support", "Resistance", "Buy limit", "Sell limit"};
    const char *tool_tags[] = {"cursor", "trend", "support", "resistance", "buy-limit", "sell-limit"};
    GtkToggleButton *group = NULL;
    for (int i = 0; i < 6; ++i) {
        GtkWidget *button = gtk_toggle_button_new_with_label(tools[i]);
        char tag[64]; (void)snprintf(tag, sizeof tag, "trading.chart.%s", tool_tags[i]);
        (void)umi_gtk4_automation_tag_widget(button, tag);
        if (group == NULL) group = GTK_TOGGLE_BUTTON(button);
        else gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button), group);
        if (i == 0) gtk_toggle_button_set_active(group, TRUE);
        g_object_set_data(G_OBJECT(button), "chart-tool", GINT_TO_POINTER(i));
        if (i >= 4) gtk_widget_set_sensitive(button, context->controller != NULL);
        gtk_widget_set_tooltip_text(button, i >= 4
            ? "Choose this tool, then click a chart price to prepare the order ticket for review."
            : i == 1 ? "Click two chart points to draw a trend line; Escape cancels an unfinished line."
            : i == 0 ? "Drag to pan. Use the mouse wheel, + or - to zoom; Home fits retained bars."
            : "Choose this tool, then click a chart price to mark a horizontal level.");
        g_signal_connect_object(button, "toggled", G_CALLBACK(ChartToolChanged), G_OBJECT(root), 0);
        gtk_box_append(GTK_BOX(rail), button);
    }
    state->area = gtk_drawing_area_new();
    (void)umi_gtk4_automation_tag_widget(state->area, "trading.chart.canvas");
    gtk_widget_set_focusable(state->area, TRUE);
    gtk_widget_set_size_request(state->area, 300, 240);
    gtk_widget_set_hexpand(state->area, TRUE); gtk_widget_set_vexpand(state->area, TRUE);
    state->references++;
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(state->area), ChartDraw, state, ChartRelease);
    gtk_box_append(GTK_BOX(body), state->area);
    GtkWidget *objects = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    state->objects = gtk_drop_down_new(NULL, NULL); gtk_widget_set_hexpand(state->objects, TRUE);
    gtk_box_append(GTK_BOX(objects), gtk_label_new("Drawings")); gtk_box_append(GTK_BOX(objects), state->objects);
    (void)umi_gtk4_automation_tag_widget(state->objects, "trading.chart.drawings");
    (void)ChartButton(objects, "Remove selected", "trading.chart.remove-drawing", G_CALLBACK(ChartRemove), root);
    gtk_box_append(GTK_BOX(root), objects);
    state->status = gtk_label_new(""); state->message = gtk_label_new("Drawings stay with this instrument during the workspace session. Chart orders prepare a ticket for review.");
    gtk_label_set_wrap(GTK_LABEL(state->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(state->status), 0); gtk_label_set_xalign(GTK_LABEL(state->message), 0);
    gtk_box_append(GTK_BOX(root), state->status); gtk_box_append(GTK_BOX(root), state->message);
    (void)umi_gtk4_automation_tag_widget(state->message, "trading.chart.message");
    GtkEventController *motion = gtk_event_controller_motion_new();
    g_signal_connect_object(motion, "motion", G_CALLBACK(ChartMotion), G_OBJECT(root), 0);
    g_signal_connect_object(motion, "leave", G_CALLBACK(ChartAreaLeave), G_OBJECT(root), 0);
    gtk_widget_add_controller(state->area, motion);
    GtkEventController *scroll = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    g_signal_connect_object(scroll, "scroll", G_CALLBACK(ChartScroll), G_OBJECT(root), 0);
    gtk_widget_add_controller(state->area, scroll);
    GtkGesture *click = gtk_gesture_click_new(); gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 1);
    g_signal_connect_object(click, "pressed", G_CALLBACK(ChartClick), G_OBJECT(root), 0);
    gtk_widget_add_controller(state->area, GTK_EVENT_CONTROLLER(click));
    GtkGesture *drag = gtk_gesture_drag_new(); gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(drag), 1);
    g_signal_connect_object(drag, "drag-begin", G_CALLBACK(ChartDragBegin), G_OBJECT(root), 0);
    g_signal_connect_object(drag, "drag-end", G_CALLBACK(ChartDragEnd), G_OBJECT(root), 0);
    gtk_widget_add_controller(state->area, GTK_EVENT_CONTROLLER(drag));
    GtkEventController *key = gtk_event_controller_key_new();
    g_signal_connect_object(key, "key-pressed", G_CALLBACK(ChartKey), G_OBJECT(root), 0);
    gtk_widget_add_controller(state->area, key);
    g_signal_connect_object(state->studies, "notify::selected", G_CALLBACK(ChartStudyChanged), G_OBJECT(root), 0);
    g_signal_connect_object(state->period, "value-changed", G_CALLBACK(ChartPeriodChanged), G_OBJECT(root), 0);
    ChartRefresh(state);
    state->timer = g_timeout_add(1000, ChartTick, state);
    return root;
}

void UmiGtk4TradingInteractiveChartRefresh(GtkWidget *chart)
{
    if (chart == NULL) return;
    InteractiveChart *state = ChartState(chart);
    if (state != NULL) ChartRefresh(state);
}
