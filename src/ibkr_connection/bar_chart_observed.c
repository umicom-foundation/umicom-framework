/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/bar_chart_observed.c
 * PURPOSE: Render broker observations with aligned volume and explicit UTC endpoint labels.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "bar_chart_private.h"
#include "umicom/chart/timeframe.h"
#include <math.h>
#include <stdio.h>

static UmiStatus TimeLabel(int64_t time, char *out, size_t capacity) {
    char minute[64];
    UmiStatus status = UmiChartTimeframeFormatUtc(time, 0, minute, sizeof minute);
    if (status != UMI_STATUS_OK)
        return status;
    int64_t within = time % INT64_C(60000);
    if (within < 0)
        within += INT64_C(60000);
    int count = snprintf(out, capacity, "%s:%02u UTC", minute, (unsigned)(within / 1000));
    return count < 0 || (size_t)count >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static UmiStatus Volumes(UmiChartRenderScene *scene, const UmiChartCandle *candles,
    const bool *available, size_t count, const UmiChartPlotViewport *price,
    const UmiChartPlotStyle *style) {
    size_t observed = 0U;
    double maximum = 0.0;
    for (size_t i = 0U; i < count; ++i) {
        if (available == NULL || available[i]) {
            ++observed;
            maximum = fmax(maximum, candles[i].volume);
        }
    }
    char label[UMI_CHART_RENDER_TEXT_CAPACITY];
    if (observed == 0U)
        (void)snprintf(label, sizeof label, "Volume unavailable | no exact provider observations");
    else
        (void)snprintf(label, sizeof label, "Volume max %.8g | %zu/%zu observed | provider units",
                       maximum, observed, count);
    UmiStatus status = umi_chart_render_scene_add_text(scene,
        (UmiChartRenderPoint){10.0, 315.0}, label, style->line_color);
    UmiChartRenderRectangle area = {price->area.x, 325.0, price->area.width, 70.0};
    if (status == UMI_STATUS_OK)
        status = umi_chart_render_scene_add_filled_rectangle(scene, area, style->background_color);
    if (status == UMI_STATUS_OK)
        status = umi_chart_render_scene_add_stroked_rectangle(scene, area,
            style->border_color, style->grid_stroke_width);
    /* Match the existing candlestick body's width and edge clamping, rather
     * than compressing away missing volume observations. Both rows share the
     * original time scale, so provider gaps stay visible in each row. */
    double width = fmax(1.0, fmin(36.0,
        (price->area.width / (double)count) * (1.0 - style->candle_gap_ratio)));
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i) {
        if (available != NULL && !available[i])
            continue;
        double x = 0.0;
        status = umi_chart_plot_map_time(price, candles[i].time_ms, &x);
        if (status != UMI_STATUS_OK)
            break;
        x = fmax(area.x + width * 0.5, fmin(area.x + area.width - width * 0.5, x));
        /* A zero observation is a baseline mark; unavailable observations
         * have no mark. The summary reports how many values were available. */
        double height = maximum > 0.0 ? (candles[i].volume / maximum) * area.height : 0.0;
        height = fmax(1.0, height);
        UmiChartColor color = umi_chart_candle_is_up(&candles[i]) ? style->positive_color : style->negative_color;
        status = umi_chart_render_scene_add_filled_rectangle(scene,
            (UmiChartRenderRectangle){x - width * 0.5, area.y + area.height - height, width, height}, color);
    }
    return status;
}

UmiStatus UmiIbkrMarketBarSceneObserved(const UmiChartCandle *candles, const bool *available,
    size_t count, const char *caption, UmiChartRenderScene **out) {
    if (candles == NULL || count == 0U || count > UMI_IBKR_HISTORICAL_BAR_LIMIT ||
        caption == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < count; ++i) {
        if (umi_chart_candle_validate(&candles[i]) != UMI_STATUS_OK ||
            (i != 0U && candles[i].time_ms <= candles[i - 1U].time_ms))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiChartRenderScene *scene = NULL;
    UmiStatus status = umi_chart_render_scene_create(count * 3U + 128U, &scene);
    UmiChartPlotViewport viewport;
    UmiChartPlotStyle style;
    umi_chart_plot_style_dark(&style);
    if (status == UMI_STATUS_OK)
        status = umi_chart_render_scene_set_coordinate_size(scene, 960.0, 440.0);
    if (status == UMI_STATUS_OK)
        status = umi_chart_plot_viewport_from_candles(candles, count,
            (UmiChartRenderRectangle){10.0, 30.0, 850.0, 260.0}, 0.08, &viewport);
    if (status == UMI_STATUS_OK)
        status = umi_chart_plot_add_frame(scene, &viewport, &style);
    if (status == UMI_STATUS_OK)
        status = umi_chart_plot_add_candlesticks(scene, candles, count, &viewport, &style);
    if (status == UMI_STATUS_OK) {
        char upper[64], lower[64];
        (void)snprintf(upper, sizeof upper, "%.8g", viewport.maximum_value);
        (void)snprintf(lower, sizeof lower, "%.8g", viewport.minimum_value);
        status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){870.0, 45.0},
                                                upper, style.line_color);
        if (status == UMI_STATUS_OK)
            status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){870.0, 285.0},
                                                    lower, style.line_color);
        if (status == UMI_STATUS_OK)
            status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){12.0, 18.0},
                                                    caption, style.line_color);
    }
    if (status == UMI_STATUS_OK)
        status = Volumes(scene, candles, available, count, &viewport, &style);
    if (status == UMI_STATUS_OK) {
        char first[96], last[96];
        status = TimeLabel(candles[0].time_ms, first, sizeof first);
        if (status == UMI_STATUS_OK)
            status = TimeLabel(candles[count - 1U].time_ms, last, sizeof last);
        if (status == UMI_STATUS_OK)
            status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){10.0, 420.0},
                                                    first, style.line_color);
        if (status == UMI_STATUS_OK && count > 1U)
            status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){620.0, 420.0},
                                                    last, style.line_color);
    }
    if (status != UMI_STATUS_OK) {
        umi_chart_render_scene_destroy(scene);
        return status;
    }
    /* The caller keeps any previously published scene until the complete
     * replacement succeeds. This scene owns all labels and drawing commands. */
    *out = scene;
    return UMI_STATUS_OK;
}
