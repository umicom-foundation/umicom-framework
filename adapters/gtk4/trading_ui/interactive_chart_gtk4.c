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
#include "umicom/chart/drawing_tools.h"
#include "umicom/trading/chart_timeframe.h"
#include "umicom/trading/chart_history.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHART_STATE "umicom-interactive-chart-state"
typedef struct InteractiveChart {
    GtkWidget *tool_buttons[9];
    GtkWidget *history_undo, *history_redo, *history_note;
    UmiChartDrawingHistorySnapshot history;
    GtkWidget *timeframe;
    char timeframe_instrument[UMI_FINANCE_ID_CAPACITY];
    int timeframe_refresh;
    char edit_id[128];
    uint64_t edit_revision;
    /* Appearance drafts bind to a copied row; ordinary chart refreshes do not
     * discard input or transfer it to a new selection. */
    char appearance_id[128], appearance_pane[128];
    uint64_t appearance_revision;
    GtkWidget *appearance_fields, *appearance_color, *appearance_width, *appearance_fill, *appearance_note;
    int references;
    UmiGtk4TradingPanelContext *context;
    UmiTradingChartPersistence *persistence;
    GtkWidget *restore_button, *preview_text;
    /* The root owns this section. Keep its identity so an explicit Preview can
     * reveal the reviewed data without replacing widgets or chart state. */
    GtkWidget *preview_section;
    uint64_t preview_id;
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

/* Native signals can still be emitted on externally retained controls after
 * teardown. The explicit detach contract makes all model actions inert. */
static int ChartAttached(const InteractiveChart *state)
{ return state != NULL && state->context != NULL && state->root != NULL; }

/* Finishing a captured move returns to Cursor. Later clicks must not silently
 * become creation gestures after the selected record or instrument changes. */
static void ChartFinishMove(InteractiveChart *state)
{
    if(state->edit_id[0]=='\0')return;
    state->edit_id[0]='\0';state->have_first=0;state->tool=0;
    if(state->tool_buttons[0]!=NULL)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state->tool_buttons[0]),TRUE);
    if(state->area!=NULL)gtk_widget_queue_draw(state->area);
}

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
/* The object list now recognises the same six tools as Framework geometry and persistence. The previous implementation remains for engineering review. */
#if 0
        if (strcmp(drawing.tool, "trend") != 0 && strcmp(drawing.tool, "support") != 0 &&
            strcmp(drawing.tool, "resistance") != 0) continue;
#endif
        UmiChartDrawingKind kind;
        if(UmiChartDrawingKindParse(drawing.tool,&kind)!=UMI_STATUS_OK)continue;
        size_t index = state->drawing_count++;
        (void)snprintf(state->drawing_ids[index], sizeof state->drawing_ids[index], "%s", drawing.id);
        state->drawing_versions[index] = drawing.revision;
        char label[196];
/* The object list now retains hidden drawings and identifies their visibility, so hiding never removes access to their identity or controls. The previous implementation remains for engineering review. */
#if 0
        (void)snprintf(label, sizeof label, "%s %.8g | %.96s%s", drawing.tool, drawing.value1,
            drawing.id, drawing.locked ? " (locked)" : "");
#endif
        (void)snprintf(label, sizeof label, "%s %.8g | %.96s%s%s", drawing.tool, drawing.value1,
            drawing.id, drawing.locked ? " (locked)" : "",
            (drawing.visibility_flags & UMI_CHART_DRAWING_VISIBILITY_HIDDEN) != 0U ? " (hidden)" : "");
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
static void ChartHistoryRefresh(InteractiveChart *state);
static void ChartHistoryAction(InteractiveChart *state, int redo);
static void ChartTimeframeRefresh(InteractiveChart *state);
static void ChartRefresh(InteractiveChart *state)
{
    if (state == NULL || state->context == NULL) return;
    UmiChartRenderScene *scene = NULL;
    UmiTradingChartSceneInfo info = {0};
    UmiStatus status = UmiTradingChartBuildScene(state->context->workspace, &info, &scene);
    if (status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND) {
        if (strcmp(state->info.instrument_id, info.instrument_id) != 0) {
            ChartFinishMove(state);
            state->have_first = 0;
            state->preview_id = 0U;
            gtk_widget_set_sensitive(state->restore_button, FALSE);
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->preview_text)), "", -1);
        }
        if (state->info.interval_ms != info.interval_ms) {
            ChartFinishMove(state); state->have_first = 0;
            state->preview_id = 0U; gtk_widget_set_sensitive(state->restore_button, FALSE);
        }
        umi_chart_render_scene_destroy(state->scene);
        state->scene = scene;
        state->info = info;
        char text[192];
/* The chart status separates retained provider bars from the displayed aggregate candles and names the active interval. The previous implementation remains for engineering review. */
#if 0
        (void)snprintf(text, sizeof text, "%s | %zu of %zu retained bars | wheel: zoom | drag: pan | UTC",
            info.instrument_id[0] ? info.instrument_id : "Waiting for retained market history",
            info.window.count, info.retained_bars);
#endif
        (void)snprintf(text, sizeof text, "%s | %s | %zu/%zu candles from %zu source bars | UTC",
            info.instrument_id[0] ? info.instrument_id : "Waiting for retained market history",
            UmiChartTimeframeName(info.interval_ms), info.window.count, info.retained_bars, info.source_bars);
        gtk_label_set_text(GTK_LABEL(state->status), text);
        ChartObjectsRefresh(state);
    } else {
        /* Never allow a gesture to use an older price mapping after a failure. */
        ChartFinishMove(state);
        umi_chart_render_scene_destroy(state->scene); state->scene = NULL;
        state->have_first = 0;
        ChartMessage(state, "Chart unavailable. Wait for valid market history.");
    }
    ChartTimeframeRefresh(state);
    ChartHistoryRefresh(state);
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
/* Pending range and zone gestures show a box preview; line and ray gestures retain a line preview. The previous implementation remains for engineering review. */
#if 0
            cairo_move_to(cr, x, y); cairo_line_to(cr, state->pointer_x, state->pointer_y); cairo_stroke(cr);
#endif
            if(state->tool==6||state->tool==7)
                cairo_rectangle(cr,fmin(x,state->pointer_x),fmin(y,state->pointer_y),fabs(state->pointer_x-x),fabs(state->pointer_y-y));
            else {cairo_move_to(cr,x,y);cairo_line_to(cr,state->pointer_x,state->pointer_y);}
            cairo_stroke(cr);
        }
    }
    char label[96];
/* Full UTC crosshair dates distinguish multi-day views and correctly format pre-epoch times through the shared formatter. The previous implementation remains for engineering review. */
#if 0
    (void)snprintf(label, sizeof label, "%.8g | %02d:%02d UTC", point.value,
        (int)((point.time_ms / 3600000) % 24), (int)((point.time_ms / 60000) % 60));
#endif
    char time[64];
    if (UmiChartTimeframeFormatUtc(point.time_ms, 0, time, sizeof time) != UMI_STATUS_OK)
        (void)snprintf(time, sizeof time, "%s", "Time unavailable");
    (void)snprintf(label, sizeof label, "%.8g | %.63s UTC", point.value, time);
    cairo_set_source_rgb(cr, 0.9, 0.93, 0.98); cairo_set_font_size(cr, 12);
    cairo_move_to(cr, 25, 38); cairo_show_text(cr, label);
    cairo_restore(cr);
}
static UmiStatus ChartNavigate(InteractiveChart *state, int zoom, int pan, int fit)
{
    if (!ChartAttached(state)) return UMI_STATUS_INVALID_STATE;
    UmiChartNavigation navigation;
    UmiStatus status = UmiTradingWorkspaceGetChartNavigation(state->context->workspace, state->info.instrument_id, &navigation);
    if (status != UMI_STATUS_OK) return status;
/* Fit resets zoom and pinning within the chosen timeframe; it no longer changes the interval to source bars. The previous implementation remains for engineering review. */
#if 0
    if (fit) navigation = (UmiChartNavigation){0};
#endif
    if (fit) {
        uint32_t interval = navigation.interval_ms;
        navigation = (UmiChartNavigation){0}; navigation.interval_ms = interval;
    }
    else {
/* Navigation must count the same aggregate candles that the renderer displays, so zoom and pan remain aligned with chart geometry. The previous implementation remains for engineering review. */
#if 0
        UmiChartCandle candles[UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY];
        size_t count = umi_trading_workspace_selected_bar_count(state->context->workspace);
        if (count == 0 || count > UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY) return UMI_STATUS_NOT_FOUND;
        for (size_t i = 0; i < count; ++i) {
            UmiBar bar;
            status = umi_trading_workspace_selected_bar_at(state->context->workspace, i, &bar);
            if (status != UMI_STATUS_OK) return status;
            candles[i] = (UmiChartCandle){bar.start_time_ms, bar.open, bar.high, bar.low, bar.close, bar.volume};
        }
#endif
        UmiChartCandle candles[UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY];
        UmiChartTimeframeSummary aggregation;
        status = UmiTradingWorkspaceBuildChartCandles(state->context->workspace, state->info.instrument_id,
            candles, UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY, &aggregation);
        if (status != UMI_STATUS_OK) return status;
        size_t count = aggregation.candle_count;
        if (count == 0U) return UMI_STATUS_NOT_FOUND;
        if (zoom != 0) status = UmiChartNavigationZoom(&navigation, candles, count, zoom);
        else status = UmiChartNavigationPan(&navigation, candles, count, pan);
        if (status != UMI_STATUS_OK) return status;
    }
    status = UmiTradingWorkspaceSetChartNavigation(state->context->workspace, state->info.instrument_id, &navigation);
    if (status == UMI_STATUS_OK) ChartRefresh(state);
    return status;
}
#include "chart_timeframe_gtk4.inc"
static void ChartNavigateClicked(GtkButton *button, gpointer root)
{
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return;
    int action = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "chart-action"));
    UmiStatus status = ChartNavigate(state, action == 1 ? 1 : action == 2 ? -1 : 0,
        action == 3 ? -10 : action == 4 ? 10 : 0, action == 5);
    if (status != UMI_STATUS_OK) ChartMessage(state, "Navigation needs retained history for this instrument.");
}
static gboolean ChartScroll(GtkEventControllerScroll *controller, double dx, double dy, gpointer root)
{
    (void)controller; (void)dx;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return FALSE;
    if (dy != 0 && isfinite(dy)) (void)ChartNavigate(state, dy < 0 ? 1 : -1, 0, 0);
    return TRUE;
}
static void ChartMotion(GtkEventControllerMotion *controller, double x, double y, gpointer root)
{
    (void)controller;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return;
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
    if (!ChartAttached(state)) return;
    state->have_pointer = 0; gtk_widget_queue_draw(state->area);
}
/* Native tool gestures now compose the shared six-tool model and guarded anchor edits. The original chart-ticket dispatch remains exclusive to Buy/Sell tools, preventing drawing tools from being interpreted as orders. The previous implementation remains for engineering review. */
#if 0
static void ChartToolChanged(GtkToggleButton *button, gpointer root)
{
    if (!gtk_toggle_button_get_active(button)) return;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return;
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
    if (!ChartAttached(state)) return;
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
#endif
static UmiChartDrawingKind ChartToolKind(int tool)
{
    switch(tool){
    case 1:return UMI_CHART_DRAWING_TREND;
    case 2:return UMI_CHART_DRAWING_SUPPORT;
    case 3:return UMI_CHART_DRAWING_RESISTANCE;
    case 6:return UMI_CHART_DRAWING_RANGE;
    case 7:return UMI_CHART_DRAWING_LIQUIDITY_ZONE;
    case 8:return UMI_CHART_DRAWING_RAY;
    default:return (UmiChartDrawingKind)0;
    }
}
static int ChartKindTool(UmiChartDrawingKind kind)
{
    for(int i=1;i<9;++i)if(ChartToolKind(i)==kind)return i;
    return 0;
}
static void ChartToolChanged(GtkToggleButton *button,gpointer root)
{
    if(!gtk_toggle_button_get_active(button))return;
    InteractiveChart *state=ChartState(root);if(!ChartAttached(state))return;
    int tool=GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button),"chart-tool"));
    if(tool<0||tool>=9)return;
    state->tool=tool;state->have_first=0;state->edit_id[0]='\0';
    static const char *messages[]={
        "Drag to pan; use the wheel or +/- buttons to zoom.",
        "Click two price points to draw a trend line.","Click a price to mark support.",
        "Click a price to mark resistance.","Click a price to prepare a Buy limit ticket.",
        "Click a price to prepare a Sell limit ticket.",
        "Click opposite corners of a range box.",
        "Click opposite corners of a liquidity annotation. It marks your analysis, not measured liquidity.",
        "Click the ray origin, then its direction. The ray extends through the second point."};
    ChartMessage(state,messages[tool]);gtk_widget_queue_draw(state->area);
}
static void ChartClick(GtkGestureClick *gesture,int presses,double x,double y,gpointer root)
{
    (void)gesture;InteractiveChart *state=ChartState(root);if(!ChartAttached(state))return;
    if(presses!=1||state->scene==NULL||state->tool==0)return;
    int width=gtk_widget_get_width(state->area),height=gtk_widget_get_height(state->area);
    if(width<=0||height<=0)return;
    UmiChartPoint point;
    if(UmiChartPlotUnmap(&state->info.price,x*UMI_TRADING_CHART_WIDTH/width,y*UMI_TRADING_CHART_HEIGHT/height,&point)!=UMI_STATUS_OK)return;
    if(state->tool==4||state->tool==5){
        if(state->context->controller==NULL){ChartMessage(state,"Order entry is unavailable in this chart view.");return;}
        char instrument[UMI_FINANCE_ID_CAPACITY];(void)snprintf(instrument,sizeof(instrument),"%s",state->info.instrument_id);
        UmiTradingUiController *controller=state->context->controller;
        UmiSide side=state->tool==4?UMI_SIDE_BUY:UMI_SIDE_SELL;
        /* The existing ticket flow may replace the panel during dispatch. */
        (void)UmiTradingUiControllerPrepareChartLimit(controller,instrument,side,point.value);return;
    }
    UmiChartDrawingKind kind=ChartToolKind(state->tool);
    const char *tool=UmiChartDrawingKindName(kind);if(tool==NULL)return;
    int twoPoints=kind!=UMI_CHART_DRAWING_SUPPORT&&kind!=UMI_CHART_DRAWING_RESISTANCE;
    if(twoPoints&&!state->have_first){
        state->first_point=point;state->have_first=1;
        ChartMessage(state,"Choose the second anchor; Escape cancels without changing the drawing.");
        gtk_widget_grab_focus(state->area);return;
    }
    int editing=state->edit_id[0]!='\0';
    UmiStatus status=editing?UmiTradingWorkspaceMoveChartDrawing(state->context->workspace,state->info.instrument_id,
        state->edit_id,state->edit_revision,twoPoints?state->first_point:point,point):
        UmiTradingWorkspaceAddChartDrawing(state->context->workspace,state->info.instrument_id,tool,
            twoPoints?state->first_point:point,point);
    if(status==UMI_STATUS_OK){
        if(editing)ChartFinishMove(state);
        state->have_first=0;
        ChartMessage(state,editing?"Drawing anchors updated. Save chart to keep them between sessions.":
            "Drawing added. Save chart to keep it between sessions.");ChartRefresh(state);
    }else{
        /* A stale edit is cancelled rather than retargeted to another row. */
        if(editing)ChartFinishMove(state);
        state->have_first=0;
        ChartMessage(state,"Drawing unchanged: invalid anchors, a lock, or changed chart evidence. Select it again.");
    }
}

static void ChartDragBegin(GtkGestureDrag *gesture, double x, double y, gpointer root)
{
    (void)gesture; (void)x; (void)y;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return;
    state->dragging = state->tool == 0;
}
static void ChartDragEnd(GtkGestureDrag *gesture, double x, double y, gpointer root)
{
    (void)gesture; (void)y;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return;
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
    if (!ChartAttached(state)) return FALSE;
    /* This controller belongs to the chart canvas, so text-entry undo remains
     * local to the focused entry. Extra Alt/Super modifiers are not intercepted. */
    if ((mods & GDK_CONTROL_MASK) != 0U && (mods & (GDK_ALT_MASK | GDK_SUPER_MASK | GDK_META_MASK)) == 0U) {
        guint lower = gdk_keyval_to_lower(key);
        if (lower == GDK_KEY_z || (lower == GDK_KEY_y && (mods & GDK_SHIFT_MASK) == 0U)) {
            ChartHistoryAction(state, lower == GDK_KEY_y || (mods & GDK_SHIFT_MASK) != 0U); return TRUE;
        }
    }
    if (key == GDK_KEY_Escape) ChartFinishMove(state);
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
    if (!ChartAttached(state)) return;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(state->objects));
    if (selected >= state->drawing_count) return;
    UmiStatus status = UmiTradingWorkspaceRemoveChartDrawing(state->context->workspace, state->info.instrument_id,
        state->drawing_ids[selected], state->drawing_versions[selected]);
    ChartMessage(state, status == UMI_STATUS_OK ? "Selected drawing removed." : "Drawing changed, is locked, or belongs to another instrument. Select it again.");
    ChartRefresh(state);
}
/* Resolve the displayed object version before each action. No current-row
 * lookup may silently redirect a retained action to another drawing. */
static UmiStatus ChartSelectedDrawing(InteractiveChart *state,UmiChartDrawingSnapshot *out)
{
    if(!ChartAttached(state))return UMI_STATUS_INVALID_STATE;
    guint selected=gtk_drop_down_get_selected(GTK_DROP_DOWN(state->objects));
    if(selected>=state->drawing_count)return UMI_STATUS_NOT_FOUND;
    UmiChartDrawingRegistry *registry=umi_chart_workspace_drawings(umi_trading_workspace_charts(state->context->workspace));
    UmiStatus status=umi_chart_drawing_registry_find(registry,state->drawing_ids[selected],out);
    if(status!=UMI_STATUS_OK)return status;
    if(out->revision!=state->drawing_versions[selected])return UMI_STATUS_BUSY;
    return strcmp(out->pane_id,state->info.instrument_id)==0?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}
#include "drawing_visibility_gtk4.inc"
static GtkWidget *ChartButton(GtkWidget *box, const char *label, const char *tag,
    GCallback callback, GtkWidget *root);
#include "drawing_appearance_gtk4.inc"
#include "drawing_history_gtk4.inc"

static void ChartObjectSelectionChanged(GObject *object,GParamSpec *spec,gpointer root)
{
    (void)object;(void)spec;InteractiveChart *state=ChartState(root);
    if(!ChartAttached(state)||state->edit_id[0]=='\0')return;
    ChartFinishMove(state);
    ChartMessage(state,"Drawing selection changed; the unfinished move was cancelled.");
}
static void ChartLockClicked(GtkButton *button,gpointer root)
{
    (void)button;InteractiveChart *state=ChartState(root);if(!ChartAttached(state))return;
    UmiChartDrawingSnapshot drawing;UmiStatus status=ChartSelectedDrawing(state,&drawing);
    if(status==UMI_STATUS_OK)status=UmiTradingWorkspaceSetChartDrawingLocked(state->context->workspace,
        state->info.instrument_id,drawing.id,drawing.revision,!drawing.locked);
    ChartFinishMove(state);state->have_first=0;ChartRefresh(state);
    ChartMessage(state,status==UMI_STATUS_OK?(drawing.locked?"Drawing unlocked.":"Drawing locked against moving and removal."):
        "Drawing changed or is unavailable. Select its current entry again.");
}
static void ChartDuplicateClicked(GtkButton *button,gpointer root)
{
    (void)button;InteractiveChart *state=ChartState(root);if(!ChartAttached(state))return;
    UmiChartDrawingSnapshot drawing;char id[128];UmiStatus status=ChartSelectedDrawing(state,&drawing);
    if(status==UMI_STATUS_OK)status=UmiTradingWorkspaceDuplicateChartDrawing(state->context->workspace,
        state->info.instrument_id,drawing.id,drawing.revision,id,sizeof(id));
    ChartFinishMove(state);state->have_first=0;ChartRefresh(state);
    if(status==UMI_STATUS_OK){
        for(size_t i=0;i<state->drawing_count;++i)if(strcmp(state->drawing_ids[i],id)==0){
            gtk_drop_down_set_selected(GTK_DROP_DOWN(state->objects),(guint)i);break;
        }
    }
    /* A copied hidden object remains hidden; direct the user to reveal it
     * before invoking the existing protected move workflow. */
    if (status == UMI_STATUS_OK && (drawing.visibility_flags & UMI_CHART_DRAWING_VISIBILITY_HIDDEN) != 0U) {
        ChartMessage(state, "Hidden unlocked copy selected. Choose Hide / Show before moving it.");
        return;
    }
    ChartMessage(state,status==UMI_STATUS_OK?"Unlocked copy selected at the same anchors. Choose Move selected to reposition it.":
        "Drawing was not duplicated. Select its current entry and check the drawing capacity.");
}
static void ChartMoveClicked(GtkButton *button,gpointer root)
{
    (void)button;InteractiveChart *state=ChartState(root);if(!ChartAttached(state))return;
    UmiChartDrawingSnapshot drawing;UmiStatus status=ChartSelectedDrawing(state,&drawing);UmiChartDrawingKind kind;
    if(status==UMI_STATUS_OK)status=UmiChartDrawingKindParse(drawing.tool,&kind);
    if(status!=UMI_STATUS_OK||drawing.locked){ChartMessage(state,"Select a current unlocked drawing before moving its anchors.");return;}
    if ((drawing.visibility_flags & UMI_CHART_DRAWING_VISIBILITY_HIDDEN) != 0U) {
        ChartMessage(state, "Show this drawing before moving its anchors.");
        return;
    }
    int tool=ChartKindTool(kind);if(tool==0)return;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state->tool_buttons[tool]),TRUE);
    state->tool=tool;state->have_first=0;state->edit_revision=drawing.revision;
    (void)snprintf(state->edit_id,sizeof(state->edit_id),"%s",drawing.id);
    ChartMessage(state,tool==2||tool==3?"Click the new level. Escape cancels this move.":
        "Click two replacement anchors. Escape cancels and retains the original drawing.");
}

static void ChartStudyApply(InteractiveChart *state)
{
    if (!ChartAttached(state)) return;
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
/* Persistence actions use the visible instrument identity. A queued click
 * cannot save or restore the newly selected instrument through an old panel. */
static int ChartPersistenceReady(InteractiveChart *state)
{
    if (!ChartAttached(state) || gtk_widget_get_root(state->root) == NULL) return 0;
    UmiChartNavigation guard;
    if (UmiTradingWorkspaceGetChartNavigation(state->context->workspace, state->info.instrument_id, &guard) != UMI_STATUS_OK) {
        ChartMessage(state, "The instrument changed. Refresh the chart before saving or restoring."); return 0;
    }
    if (state->persistence == NULL) { ChartMessage(state, "Chart storage is not available in this window."); return 0; }
    return 1;
}
static void ChartSaveClicked(GtkButton *button, gpointer root)
{
    (void)button; InteractiveChart *state = ChartState(root);
    if (!ChartPersistenceReady(state)) return;
    UmiChartCheckpointReport report;
    UmiStatus status = UmiTradingChartPersistenceSave(state->persistence, state->info.instrument_id,
        (uint64_t)(g_get_real_time() / 1000), &report);
    if (status == UMI_STATUS_OK) {
        state->preview_id = 0U; gtk_widget_set_sensitive(state->restore_button, FALSE);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->preview_text)), "", -1);
        ChartMessage(state, report.durable ? "Chart drawings and view saved to this profile on disk."
            : "Chart saved in memory only; this copy will not survive closing the session.");
    } else if (status == UMI_STATUS_INVALID_STATE) {
        ChartMessage(state, "A saved chart already exists or changed. Preview saved chart, then save again only if you want to replace it.");
    } else {
        char message[160]; (void)snprintf(message, sizeof(message),
            "Chart was not saved (status %d). Preview storage to check recovery; your current chart is unchanged.", (int)status);
        ChartMessage(state, message);
    }
}
static void ChartPreviewClicked(GtkButton *button, gpointer root)
{
    (void)button; InteractiveChart *state = ChartState(root);
    if (!ChartPersistenceReady(state)) return;
    state->preview_id = 0U; gtk_widget_set_sensitive(state->restore_button, FALSE);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->preview_text));
    gtk_text_buffer_set_text(buffer, "", -1);
    UmiTradingChartPreview preview;
    UmiStatus status = UmiTradingChartPersistencePreview(state->persistence, state->info.instrument_id, &preview);
    if (status != UMI_STATUS_OK) {
        ChartMessage(state, status == UMI_STATUS_NOT_FOUND ? "No saved chart for this instrument. Save chart keeps the current drawings and view."
            : "The saved chart could not be read safely. Your current chart is unchanged; check the profile storage."); return;
    }
    GString *text = g_string_new(NULL);
/* Saved navigation counts displayed candles; the review explains that count together with the persisted interval. The previous implementation remains for engineering review. */
#if 0
    g_string_append_printf(text, "Instrument: %s\nSaved drawings: %zu; current drawings: %zu\nVisible bars: %zu (0 means fit all retained bars)\nAnchor: %lld ms UTC; historical view: %s\nSaved at: %llu ms since the Unix epoch\nStorage: %s%s\n\n",
        preview.saved.pane_id, preview.saved.drawing_count, preview.current.drawing_count,
        preview.saved.navigation.visible_bars, (long long)preview.saved.navigation.anchor_ms,
        preview.saved.navigation.pinned ? "yes" : "no", (unsigned long long)preview.storage.saved_at_ms,
        preview.storage.durable ? "disk" : "memory only", preview.storage.recovered_last_good ? " (previous valid copy)" : "");
#endif
    g_string_append_printf(text, "Instrument: %s\nSaved drawings: %zu; current drawings: %zu\nVisible candles: %zu (0 means fit all candles in the saved timeframe)\nAnchor: %lld ms UTC; historical view: %s\nSaved at: %llu ms since the Unix epoch\nStorage: %s%s\n\n",
        preview.saved.pane_id, preview.saved.drawing_count, preview.current.drawing_count,
        preview.saved.navigation.visible_bars, (long long)preview.saved.navigation.anchor_ms,
        preview.saved.navigation.pinned ? "yes" : "no", (unsigned long long)preview.storage.saved_at_ms,
        preview.storage.durable ? "disk" : "memory only", preview.storage.recovered_last_good ? " (previous valid copy)" : "");
    g_string_append_printf(text, "Saved timeframe: %s; current timeframe: %s\nRetained observations only; edge buckets may be incomplete.\n\n",
        UmiChartTimeframeName(preview.saved.navigation.interval_ms), UmiChartTimeframeName(preview.current.navigation.interval_ms));
    for (size_t i = 0U; i < preview.saved.drawing_count; ++i) {
        UmiChartDrawingSnapshot drawing;
        status = UmiTradingChartPersistencePreviewDrawing(state->persistence, preview.preview_id, i, &drawing);
        if (status != UMI_STATUS_OK) break;
/* Saved-chart review now includes visibility alongside retained geometry, selection and locking before an explicit restore. The previous implementation remains for engineering review. */
#if 0
        g_string_append_printf(text, "%s | %s | (%lld, %.17g) to (%lld, %.17g)%s%s\nStyle: %s\n",
            drawing.id, drawing.tool, (long long)drawing.time1, drawing.value1,
            (long long)drawing.time2, drawing.value2, drawing.locked ? " | locked" : "",
            drawing.selected ? " | selected" : "", drawing.style);
#endif
        g_string_append_printf(text, "%s | %s | (%lld, %.17g) to (%lld, %.17g)%s%s%s\nStyle: %s\n",
            drawing.id, drawing.tool, (long long)drawing.time1, drawing.value1,
            (long long)drawing.time2, drawing.value2, drawing.locked ? " | locked" : "",
            drawing.selected ? " | selected" : "",
            (drawing.visibility_flags & UMI_CHART_DRAWING_VISIBILITY_HIDDEN) != 0U ? " | hidden" : " | visible", drawing.style);
    }
    if (status == UMI_STATUS_OK) {
        char *display = g_utf8_make_valid(text->str, -1); gtk_text_buffer_set_text(buffer, display, -1); g_free(display);
        state->preview_id = preview.preview_id; gtk_widget_set_sensitive(state->restore_button, TRUE);
        ChartMessage(state, preview.storage.recovered_last_good
            ? "Previous valid copy available. Review Saved chart details. Restore replaces this instrument's current drawings, including locked drawings, and view. Damaged storage remains unchanged."
            : "Review Saved chart details. Restore replaces this instrument's current drawings, including locked drawings, and view. Save instead replaces the saved copy with your current chart.");
    } else ChartMessage(state, "Preview changed. Preview the saved chart again before restoring.");
    g_string_free(text, TRUE);
    /* A successful Preview should show the evidence the user must review
     * before Restore. GTK does not mount a collapsed expander's body in its
     * child tree. Reveal this existing section only after a complete preview;
     * this changes presentation, never drawings, saved data or an order. */
    if (status == UMI_STATUS_OK)
        gtk_expander_set_expanded(GTK_EXPANDER(state->preview_section), TRUE);
}
static void ChartRestoreClicked(GtkButton *button, gpointer root)
{
    (void)button; InteractiveChart *state = ChartState(root);
    if (!ChartPersistenceReady(state) || state->preview_id == 0U) return;
    UmiStatus status = UmiTradingChartPersistenceRestore(state->persistence, state->info.instrument_id, state->preview_id);
    state->preview_id = 0U; gtk_widget_set_sensitive(state->restore_button, FALSE);
    if (status == UMI_STATUS_OK) { state->have_first = 0; ChartRefresh(state); }
    ChartMessage(state, status == UMI_STATUS_OK ? "Reviewed chart drawings and view restored. Prices and orders are unchanged."
        : "The chart, saved copy or preview changed. Nothing was restored. Preview again to review the latest state.");
}

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
    ChartTimeframeCreate(state, top);
    const char *studies[] = {"Candles", "SMA", "EMA", NULL};
    state->studies = gtk_drop_down_new_from_strings(studies);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(state->studies), (guint)snapshot.chart_study);
    state->period = gtk_spin_button_new_with_range(2, 200, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(state->period), (double)snapshot.chart_study_period);
/* Studies consume the same aggregate candles as the price chart, so period help names the selected timeframe. The previous implementation remains for engineering review. */
#if 0
    gtk_widget_set_tooltip_text(state->period, "Moving average period in retained bars");
#endif
    gtk_widget_set_tooltip_text(state->period, "Moving average period in candles of the selected timeframe");
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
        state->tool_buttons[i]=button;
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
    const char *extraLabels[]={"Range", "Liquidity zone", "Ray"};
    const char *extraTags[]={"range", "liquidity-zone", "ray"};
    for(int i=0;i<3;++i){
        GtkWidget *button=gtk_toggle_button_new_with_label(extraLabels[i]);state->tool_buttons[i+6]=button;
        gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),group);
        char tag[80];(void)snprintf(tag,sizeof(tag),"trading.chart.%s",extraTags[i]);
        (void)umi_gtk4_automation_tag_widget(button,tag);
        g_object_set_data(G_OBJECT(button),"chart-tool",GINT_TO_POINTER(i+6));
        gtk_widget_set_tooltip_text(button,i==2?"Click the origin, then the direction point. The ray extends through that point.":
            i==1?"Mark your own liquidity analysis with two corners. This is an annotation, not a liquidity measurement.":
            "Click two opposite corners to mark a time and price range.");
        g_signal_connect_object(button,"toggled",G_CALLBACK(ChartToolChanged),G_OBJECT(root),0);
        gtk_box_append(GTK_BOX(rail),button);
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
    GtkWidget *objectActions=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,4);
    (void)ChartButton(objectActions,"Move selected","trading.chart.move-drawing",G_CALLBACK(ChartMoveClicked),root);
    (void)ChartButton(objectActions,"Lock / Unlock","trading.chart.lock-drawing",G_CALLBACK(ChartLockClicked),root);
    (void)ChartButton(objectActions,"Duplicate selected","trading.chart.duplicate-drawing",G_CALLBACK(ChartDuplicateClicked),root);
    (void)ChartButton(objectActions, "Hide / Show", "trading.chart.hide-drawing", G_CALLBACK(ChartHideClicked), root);
    GtkWidget *hideAll = ChartButton(objectActions, "Hide all", "trading.chart.hide-all", G_CALLBACK(ChartPaneVisibilityClicked), root);
    g_object_set_data(G_OBJECT(hideAll), "hide-drawings", GINT_TO_POINTER(1));
    (void)ChartButton(objectActions, "Show all", "trading.chart.show-all", G_CALLBACK(ChartPaneVisibilityClicked), root);
    gtk_box_append(GTK_BOX(root),objectActions);
    ChartAppearanceCreate(state);
    ChartHistoryCreate(state);
    g_signal_connect_object(state->objects,"notify::selected",G_CALLBACK(ChartObjectSelectionChanged),G_OBJECT(root),0);

/* Chart drawings now have explicit profile persistence. Save and reviewed Restore replace the session-only guidance while order drafting remains unchanged. The previous implementation remains for engineering review. */
#if 0
    state->status = gtk_label_new(""); state->message = gtk_label_new("Drawings stay with this instrument during the workspace session. Chart orders prepare a ticket for review.");
#endif
    /* Chart persistence borrows the Framework service. It never saves on a
     * timer, restores on selection, or changes an order as a side effect. */
    GtkWidget *persistence = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    (void)ChartButton(persistence, "Save chart", "trading.chart.save", G_CALLBACK(ChartSaveClicked), root);
    (void)ChartButton(persistence, "Preview saved chart", "trading.chart.preview", G_CALLBACK(ChartPreviewClicked), root);
    state->restore_button = ChartButton(persistence, "Restore preview", "trading.chart.restore", G_CALLBACK(ChartRestoreClicked), root);
    gtk_widget_set_sensitive(state->restore_button, FALSE); gtk_box_append(GTK_BOX(root), persistence);
    GtkWidget *details = gtk_expander_new("Saved chart details");
    state->preview_section = details;
    (void)umi_gtk4_automation_tag_widget(details, "trading.chart.saved-review");
    GtkWidget *scroll_preview = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_preview), 120);
    gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(scroll_preview), 200);
    state->preview_text = gtk_text_view_new(); gtk_text_view_set_editable(GTK_TEXT_VIEW(state->preview_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(state->preview_text), GTK_WRAP_WORD_CHAR);
    (void)umi_gtk4_automation_tag_widget(state->preview_text, "trading.chart.saved-details");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_preview), state->preview_text);
    gtk_expander_set_child(GTK_EXPANDER(details), scroll_preview); gtk_box_append(GTK_BOX(root), details);
    state->status = gtk_label_new(""); state->message = gtk_label_new("Save chart keeps this instrument's drawings and view. Preview a saved chart before restoring it. Chart orders prepare a ticket for review.");
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

void UmiGtk4TradingInteractiveChartBindPersistence(GtkWidget *chart, UmiTradingChartPersistence *service)
{
    if (chart == NULL) return;
    InteractiveChart *state = ChartState(chart);
    if (!ChartAttached(state)) return;
    state->persistence = service; state->preview_id = 0U;
    gtk_widget_set_sensitive(state->restore_button, FALSE);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->preview_text)), "", -1);
}
void UmiGtk4TradingInteractiveChartDetach(GtkWidget *chart)
{
    if (chart == NULL) return;
    InteractiveChart *state = ChartState(chart);
    if (state == NULL) return;
    if (state->timer != 0U) g_source_remove(state->timer);
    state->timer = 0U; state->context = NULL; state->persistence = NULL;
    state->preview_id = 0U; state->have_first = 0;
    gtk_widget_set_sensitive(chart, FALSE);
}
