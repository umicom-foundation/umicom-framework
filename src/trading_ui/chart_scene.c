/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_ui/chart_scene.c
 * PURPOSE: Render price, volume, drawings and order levels without GUI-owned market logic.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading_ui/chart_scene.h"
#include "umicom/chart/indicator.h"
#include "umicom/chart/drawing_tools.h"
#include "umicom/trading/chart_timeframe.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Clip in data coordinates before projection. Clamping the two endpoints
 * independently would bend a trend or study at the visible plot boundaries. */
static UmiStatus ChartSegment(UmiChartRenderScene *scene, const UmiChartPlotViewport *v,
    UmiChartPoint a, UmiChartPoint b, UmiChartColor color, double width)
{
    long double x1 = a.time_ms, y1 = a.value;
    long double dx = (long double)b.time_ms - a.time_ms, dy = (long double)b.value - a.value;
    long double p[4] = {-dx, dx, -dy, dy};
    long double q[4] = {x1 - v->start_ms, v->end_ms - x1, y1 - v->minimum_value, v->maximum_value - y1};
    long double low = 0, high = 1;
    if (!isfinite(a.value) || !isfinite(b.value)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0; i < 4U; ++i) {
        if (p[i] == 0) { if (q[i] < 0) return UMI_STATUS_OK; continue; }
        long double ratio = q[i] / p[i];
        if (p[i] < 0) { if (ratio > high) return UMI_STATUS_OK; if (ratio > low) low = ratio; }
        else { if (ratio < low) return UMI_STATUS_OK; if (ratio < high) high = ratio; }
    }
    long double span = (long double)v->end_ms - v->start_ms;
    UmiChartRenderPoint first = {
        span == 0 ? v->area.x + v->area.width / 2 : v->area.x + (double)((x1 + low * dx - v->start_ms) / span) * v->area.width,
        v->area.y + (double)((v->maximum_value - y1 - low * dy) / ((long double)v->maximum_value - v->minimum_value)) * v->area.height};
    UmiChartRenderPoint last = {
        span == 0 ? v->area.x + v->area.width / 2 : v->area.x + (double)((x1 + high * dx - v->start_ms) / span) * v->area.width,
        v->area.y + (double)((v->maximum_value - y1 - high * dy) / ((long double)v->maximum_value - v->minimum_value)) * v->area.height};
    return umi_chart_render_scene_add_line(scene, first, last, color, width);
}
static UmiStatus ChartLevel(UmiChartRenderScene *scene, const UmiChartPlotViewport *v,
    double price, const char *label, UmiChartColor color)
{
    if (!isfinite(price)) return UMI_STATUS_INVALID_ARGUMENT;
    if (price < v->minimum_value || price > v->maximum_value) return UMI_STATUS_OK;
    double y;
    UmiStatus status = umi_chart_plot_map_value(v, price, &y);
    /* A horizontal price level spans the plot even when only one candle is
     * retained and there is no elapsed time to project horizontally. */
    if (status == UMI_STATUS_OK) status = umi_chart_render_scene_add_line(scene,
        (UmiChartRenderPoint){v->area.x, y},
        (UmiChartRenderPoint){v->area.x + v->area.width, y}, color, 1.4);
    if (status == UMI_STATUS_OK) status = umi_chart_render_scene_add_text(scene,
        (UmiChartRenderPoint){v->area.x + 8, y - 4}, label, color);
    return status;
}
UmiStatus UmiTradingChartBuildScene(UmiTradingWorkspace *workspace,
    UmiTradingChartSceneInfo *out_info, UmiChartRenderScene **out_scene)
{
    if (out_scene == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_scene = NULL;
    if (workspace == NULL || out_info == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = umi_trading_workspace_snapshot(workspace, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    size_t count = umi_trading_workspace_selected_bar_count(workspace);
/* An empty price history still belongs to the selected instrument. Publish that identity so object lists and guarded edits do not lose their pane; the scene remains null. The previous implementation remains for engineering review. */
#if 0
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
#endif
    if (count == 0U) {
        UmiTradingChartSceneInfo empty = {0};
        (void)snprintf(empty.instrument_id, sizeof empty.instrument_id, "%s", snapshot.selected_instrument_id);
        UmiChartNavigation navigation;
        if (UmiTradingWorkspaceGetChartNavigation(workspace, snapshot.selected_instrument_id, &navigation) == UMI_STATUS_OK)
            empty.interval_ms = navigation.interval_ms;
        *out_info = empty;
        return UMI_STATUS_NOT_FOUND;
    }
    if (count > UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY) return UMI_STATUS_INVALID_STATE;
/* Candles, volume and studies now share the selected timeframe projection; provider bars remain canonical and unchanged. The previous implementation remains for engineering review. */
#if 0
    UmiChartCandle candles[UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY];
    for (size_t i = 0; i < count; ++i) {
        UmiBar bar;
        status = umi_trading_workspace_selected_bar_at(workspace, i, &bar);
        if (status != UMI_STATUS_OK) return status;
        candles[i] = (UmiChartCandle){bar.start_time_ms, bar.open, bar.high, bar.low, bar.close, bar.volume};
    }
#endif
    UmiChartCandle candles[UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY];
    UmiChartTimeframeSummary aggregation;
    status = UmiTradingWorkspaceBuildChartCandles(workspace, snapshot.selected_instrument_id,
        candles, UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY, &aggregation);
    if (status != UMI_STATUS_OK) return status;
    count = aggregation.candle_count;
    UmiChartNavigation navigation;
    UmiTradingChartSceneInfo info = {0};
    info.retained_bars = count;
    info.source_bars = aggregation.source_count; info.interval_ms = aggregation.interval_ms;
    (void)snprintf(info.instrument_id, sizeof info.instrument_id, "%s", snapshot.selected_instrument_id);
    status = UmiTradingWorkspaceGetChartNavigation(workspace, info.instrument_id, &navigation);
    if (status == UMI_STATUS_OK) status = UmiChartNavigationResolve(&navigation, candles, count, &info.window);
    if (status == UMI_STATUS_OK) status = umi_chart_plot_viewport_from_candles(candles + info.window.first,
        info.window.count, (UmiChartRenderRectangle){18, 22, 876, 388}, 0.08, &info.price);
    if (status != UMI_STATUS_OK) return status;
    UmiChartDrawingRegistry *drawings = umi_chart_workspace_drawings(umi_trading_workspace_charts(workspace));
    size_t drawing_count = umi_chart_drawing_registry_count(drawings);
    UmiChartRenderScene *scene = NULL;
    UmiChartSeries *input = NULL, *output = NULL;
    /* Four commands per candle plus bounded levels, registry drawings and axes. */
    status = umi_chart_render_scene_create(count * 4U + drawing_count * 2U +
        UMI_TRADING_MAX_ORDERS * 4U + 160U, &scene);
    if (status != UMI_STATUS_OK) return status;
#define CHART_TRY(call) do { status = (call); if (status != UMI_STATUS_OK) goto done; } while (0)
    CHART_TRY(umi_chart_render_scene_set_coordinate_size(scene, UMI_TRADING_CHART_WIDTH, UMI_TRADING_CHART_HEIGHT));
    UmiChartPlotStyle style;
    umi_chart_plot_style_dark(&style);
    CHART_TRY(umi_chart_render_scene_add_filled_rectangle(scene,
        (UmiChartRenderRectangle){0, 0, UMI_TRADING_CHART_WIDTH, UMI_TRADING_CHART_HEIGHT}, style.background_color));
    CHART_TRY(umi_chart_plot_add_frame(scene, &info.price, &style));
    CHART_TRY(umi_chart_plot_add_candlesticks(scene, candles + info.window.first, info.window.count, &info.price, &style));
    UmiChartColor text = {0.75, 0.79, 0.85, 1.0}, amber = {0.98, 0.72, 0.20, 1.0};
    char label[UMI_CHART_RENDER_TEXT_CAPACITY];
    for (size_t i = 0; i <= 4U; ++i) {
        double value = info.price.maximum_value - (double)i / 4 * (info.price.maximum_value - info.price.minimum_value);
        (void)snprintf(label, sizeof label, "%.8g", value);
        CHART_TRY(umi_chart_render_scene_add_text(scene,
            (UmiChartRenderPoint){902, info.price.area.y + (double)i / 4 * info.price.area.height + 4}, label, text));
        size_t at = info.window.first + (info.window.count - 1U) * i / 4U;
/* Date-aware UTC labels distinguish daily candles and historical dates without depending on the process timezone. The previous implementation remains for engineering review. */
#if 0
        int64_t seconds = candles[at].time_ms / 1000;
        (void)snprintf(label, sizeof label, "%02d:%02d", (int)((seconds / 3600) % 24), (int)((seconds / 60) % 60));
        CHART_TRY(umi_chart_render_scene_add_text(scene,
            (UmiChartRenderPoint){18 + (double)i / 4 * 856, 545}, label, text));
#endif
        CHART_TRY(UmiChartTimeframeFormatUtc(candles[at].time_ms, info.interval_ms >= 86400000U, label, sizeof label));
        CHART_TRY(umi_chart_render_scene_add_text(scene,
            (UmiChartRenderPoint){18 + (double)i / 4 * 760, 545}, label, text));
    }
    CHART_TRY(umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){902, 545}, "UTC", text));
    double maximum_volume = 0;
    for (size_t i = info.window.first; i < info.window.first + info.window.count; ++i)
        if (candles[i].volume > maximum_volume) maximum_volume = candles[i].volume;
    (void)snprintf(label, sizeof label, "Volume (max %.8g)", maximum_volume);
    CHART_TRY(umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){22, 437}, label, text));
    double bar_width = fmax(1, fmin(20, 876.0 / (double)info.window.count * 0.7));
    for (size_t i = info.window.first; maximum_volume > 0 && i < info.window.first + info.window.count; ++i) {
        double x;
        CHART_TRY(umi_chart_plot_map_time(&info.price, candles[i].time_ms, &x));
        double height = candles[i].volume / maximum_volume * 78;
        if (height > 0) CHART_TRY(umi_chart_render_scene_add_filled_rectangle(scene,
            (UmiChartRenderRectangle){fmax(18, fmin(894 - bar_width, x - bar_width / 2)), 522 - height, bar_width, height},
            candles[i].close >= candles[i].open ? style.positive_color : style.negative_color));
    }
    if (snapshot.chart_study != UMI_TRADING_CHART_STUDY_NONE) {
        input = calloc(1, sizeof *input); output = calloc(1, sizeof *output);
        if (input == NULL || output == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
        CHART_TRY(umi_chart_series_init(input, "close", UMI_CHART_LINE));
        for (size_t i = 0; i < count; ++i)
            CHART_TRY(umi_chart_series_add(input, (UmiChartPoint){candles[i].time_ms, candles[i].close}));
        /* Compute studies over full retained history, then clip for zoom. */
        if (snapshot.chart_study == UMI_TRADING_CHART_STUDY_SIMPLE_AVERAGE)
            CHART_TRY(umi_chart_indicator_sma(input, snapshot.chart_study_period, output));
        else CHART_TRY(umi_chart_indicator_ema(input, snapshot.chart_study_period, output));
        for (size_t i = 1; i < output->point_count; ++i)
            CHART_TRY(ChartSegment(scene, &info.price, output->points[i - 1U], output->points[i], amber, 1.7));
    }
    for (size_t i = 0; i < drawing_count; ++i) {
        UmiChartDrawingSnapshot drawing;
        CHART_TRY(umi_chart_drawing_registry_at(drawings, i, &drawing));
        if (strcmp(drawing.pane_id, info.instrument_id) != 0) continue;
/* Shared drawing projection now clips directional rays and range/zone boxes as well as the original line tools. Unknown tool records remain retained and undisplayed, as before. The previous implementation remains for engineering review. */
#if 0
        if (strcmp(drawing.tool, "trend") == 0) {
            CHART_TRY(ChartSegment(scene, &info.price, (UmiChartPoint){drawing.time1, drawing.value1},
                (UmiChartPoint){drawing.time2, drawing.value2}, amber, 2.0));
        } else if (strcmp(drawing.tool, "support") == 0 || strcmp(drawing.tool, "resistance") == 0) {
            (void)snprintf(label, sizeof label, "%s %.8g", drawing.tool, drawing.value1);
            CHART_TRY(ChartLevel(scene, &info.price, drawing.value1, label,
                strcmp(drawing.tool, "support") == 0 ? style.positive_color : style.negative_color));
        }
#endif
        UmiChartDrawingKind kind;
        if(UmiChartDrawingKindParse(drawing.tool,&kind)==UMI_STATUS_UNAVAILABLE)continue;
        CHART_TRY(UmiChartDrawingRender(scene,&drawing,&info.price,&style));
    }
    for (size_t i = 0; i < snapshot.order_count; ++i) {
        UmiOrder order;
        CHART_TRY(UmiTradingWorkspaceOrderAt(workspace, i, &order));
        if (strcmp(order.request.instrument.instrument_id.value, info.instrument_id) != 0 ||
            (order.status != UMI_ORDER_ACCEPTED && order.status != UMI_ORDER_PARTIALLY_FILLED)) continue;
        if (order.request.type == UMI_ORDER_LIMIT || order.request.type == UMI_ORDER_STOP_LIMIT) {
            (void)snprintf(label, sizeof label, "%s limit %.8g | %.40s", order.request.side == UMI_SIDE_BUY ? "Buy" : "Sell",
                order.request.limit_price, order.request.client_order_id.value);
            CHART_TRY(ChartLevel(scene, &info.price, order.request.limit_price, label, (UmiChartColor){0.35, 0.58, 1, 1}));
        }
        if (order.request.type == UMI_ORDER_STOP || order.request.type == UMI_ORDER_STOP_LIMIT) {
            (void)snprintf(label, sizeof label, "%s stop %.8g | %.40s", order.request.side == UMI_SIDE_BUY ? "Buy" : "Sell",
                order.request.stop_price, order.request.client_order_id.value);
            CHART_TRY(ChartLevel(scene, &info.price, order.request.stop_price, label, style.negative_color));
        }
    }
    if (snapshot.draft_order.type == UMI_ORDER_LIMIT && snapshot.draft_order.limit_price > 0 &&
        strcmp(snapshot.draft_order.instrument.instrument_id.value, info.instrument_id) == 0) {
        (void)snprintf(label, sizeof label, "DRAFT %s %.8g @ %.8g", snapshot.draft_order.side == UMI_SIDE_BUY ? "Buy" : "Sell",
            snapshot.draft_order.quantity, snapshot.draft_order.limit_price);
        CHART_TRY(ChartLevel(scene, &info.price, snapshot.draft_order.limit_price, label, amber));
    }
    *out_info = info;
    *out_scene = scene;
    scene = NULL;
done:
    free(input); free(output);
    umi_chart_render_scene_destroy(scene);
    return status;
#undef CHART_TRY
}
