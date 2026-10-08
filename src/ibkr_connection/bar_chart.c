/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/bar_chart.c
 * PURPOSE: Render finite and streaming series through the same existing chart primitives.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "bar_chart_private.h"
#include <stdio.h>
static double DisplayValue(UmiDecimal value)
{
    double result = (double)value.coefficient;
    for (uint8_t i = 0U; i < value.scale; ++i)
        result /= 10.0;
    return result;
}

/* Decimal arithmetic remains authoritative in broker records. Conversion here
 * is only for display; an unrepresentable price cannot become a rounded order. */
UmiStatus UmiIbkrMarketBarCandle(const UmiIbkrHistoricalBar *bar, UmiChartCandle *out, bool *volumeAvailable)
{
    if (!bar || !out || !volumeAvailable)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status;
    if (!bar->open.exact || !bar->high.exact || !bar->low.exact || !bar->close.exact)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiChartCandle candle = {
        bar->timeMilliseconds,          DisplayValue(bar->open.value),
        DisplayValue(bar->high.value),  DisplayValue(bar->low.value),
        DisplayValue(bar->close.value), bar->volumeAvailable ? DisplayValue(bar->volume.value) : 0.0};
    status = umi_chart_candle_validate(&candle);
    if (status != UMI_STATUS_OK)
        return status;
    *out = candle;
    *volumeAvailable = bar->volumeAvailable;
    return UMI_STATUS_OK;
}
/* The scene copies every command. Callers can release their candle buffer once
 * this returns, and must recheck freshness before displaying a cached scene. */
UmiStatus UmiIbkrMarketBarScene(const UmiChartCandle *candles, size_t count, const char *caption,
                                UmiChartRenderScene **out)
{
    if (!candles || !count || count > UMI_IBKR_HISTORICAL_BAR_LIMIT || !caption || !out)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status;
    UmiChartRenderScene *scene = NULL;
    status = umi_chart_render_scene_create(count * 3U + 128U, &scene);
    UmiChartPlotViewport viewport;
    UmiChartPlotStyle style;
    umi_chart_plot_style_dark(&style);
    if (status == UMI_STATUS_OK)
        status = umi_chart_render_scene_set_coordinate_size(scene, 960.0, 340.0);
    if (status == UMI_STATUS_OK)
        status = umi_chart_plot_viewport_from_candles(
            candles, count, (UmiChartRenderRectangle){10.0, 30.0, 850.0, 275.0}, 0.08, &viewport);
    if (status == UMI_STATUS_OK)
        status = umi_chart_plot_add_frame(scene, &viewport, &style);
    if (status == UMI_STATUS_OK)
        status = umi_chart_plot_add_candlesticks(scene, candles, count, &viewport, &style);
    /* Price labels make the same scene useful to a native canvas or exporter.
     * Metadata remains explicit; a finished response is not a live price feed. */
    if (status == UMI_STATUS_OK)
    {
        char upper[64], lower[64];
        (void)snprintf(upper, sizeof upper, "%.8g", viewport.maximum_value);
        (void)snprintf(lower, sizeof lower, "%.8g", viewport.minimum_value);
        status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){870.0, 45.0}, upper,
                                                 style.line_color);
        if (status == UMI_STATUS_OK)
            status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){870.0, 300.0}, lower,
                                                     style.line_color);
        if (status == UMI_STATUS_OK)
            status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){12.0, 18.0}, caption,
                                                     style.line_color);
    }
    if (status != UMI_STATUS_OK)
    {
        umi_chart_render_scene_destroy(scene);
        return status;
    }
    *out = scene;
    return UMI_STATUS_OK;
}
