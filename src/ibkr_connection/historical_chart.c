/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/historical_chart.c
 * PURPOSE: Build owned historical candlestick scenes without introducing a parallel chart implementation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/historical_chart.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "bar_chart_private.h"
/* Retain the historical-only renderer for review. The shared projection below
 * preserves its appearance and ownership while serving streaming charts too. */
#if 0
static double DisplayValue(UmiDecimal value)
{
    double result = (double)value.coefficient;
    for (uint8_t i = 0U; i < value.scale; ++i)
        result /= 10.0;
    return result;
}
UmiStatus UmiIbkrHistoricalCandleCopy(const UmiIbkrConnection *c, uint32_t request, size_t index,
                                      uint64_t now, uint64_t age, UmiChartCandle *out, bool *volumeAvailable)
{
    if (!out || !volumeAvailable)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrHistoricalSnapshot s;
    UmiStatus status = UmiIbkrHistoricalCopy(c, request, now, age, &s);
    if (status != UMI_STATUS_OK)
        return status;
    if (s.stale || !s.complete)
        return UMI_STATUS_INVALID_STATE;
    UmiIbkrHistoricalBar bar;
    status = UmiIbkrHistoricalBarCopy(c, request, index, &bar);
    if (status != UMI_STATUS_OK)
        return status;
    if (!bar.open.exact || !bar.high.exact || !bar.low.exact || !bar.close.exact)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiChartCandle candle = {
        bar.timeMilliseconds,          DisplayValue(bar.open.value),
        DisplayValue(bar.high.value),  DisplayValue(bar.low.value),
        DisplayValue(bar.close.value), bar.volumeAvailable ? DisplayValue(bar.volume.value) : 0.0};
    status = umi_chart_candle_validate(&candle);
    if (status != UMI_STATUS_OK)
        return status;
    *out = candle;
    *volumeAvailable = bar.volumeAvailable;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrHistoricalSceneCreate(const UmiIbkrConnection *c, uint32_t request, size_t first,
                                       size_t count, uint64_t now, uint64_t age, UmiChartRenderScene **out)
{
    if (!out || !count || count > UMI_IBKR_HISTORICAL_BAR_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrHistoricalSnapshot s;
    UmiStatus status = UmiIbkrHistoricalCopy(c, request, now, age, &s);
    if (status != UMI_STATUS_OK)
        return status;
    if (s.stale || !s.complete)
        return UMI_STATUS_INVALID_STATE;
    if (first > s.count || count > s.count - first)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartCandle *candles = calloc(count, sizeof *candles);
    if (!candles)
        return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < count; ++i)
    {
        bool volume;
        status = UmiIbkrHistoricalCandleCopy(c, request, first + i, now, age, &candles[i], &volume);
        if (status != UMI_STATUS_OK)
        {
            free(candles);
            return status;
        }
    }
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
            status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){12.0, 18.0},
                                                     "Historical capture | last bar may be unfinished",
                                                     style.line_color);
    }
    free(candles);
    if (status != UMI_STATUS_OK)
    {
        umi_chart_render_scene_destroy(scene);
        return status;
    }
    *out = scene;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiIbkrHistoricalCandleCopy(const UmiIbkrConnection *c, uint32_t request, size_t index,
                                      uint64_t now, uint64_t age, UmiChartCandle *out, bool *volumeAvailable)
{
    if (!out || !volumeAvailable)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrHistoricalSnapshot s;
    UmiStatus status = UmiIbkrHistoricalCopy(c, request, now, age, &s);
    if (status != UMI_STATUS_OK)
        return status;
    if (s.stale || !s.complete)
        return UMI_STATUS_INVALID_STATE;
    UmiIbkrHistoricalBar bar;
    status = UmiIbkrHistoricalBarCopy(c, request, index, &bar);
    if (status != UMI_STATUS_OK)
        return status;
    return UmiIbkrMarketBarCandle(&bar,out,volumeAvailable);
}

UmiStatus UmiIbkrHistoricalSceneCreate(const UmiIbkrConnection *c, uint32_t request, size_t first,
                                       size_t count, uint64_t now, uint64_t age, UmiChartRenderScene **out)
{
    if (!out || !count || count > UMI_IBKR_HISTORICAL_BAR_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrHistoricalSnapshot s;
    UmiStatus status = UmiIbkrHistoricalCopy(c, request, now, age, &s);
    if (status != UMI_STATUS_OK)
        return status;
    if (s.stale || !s.complete)
        return UMI_STATUS_INVALID_STATE;
    if (first > s.count || count > s.count - first)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartCandle *candles = calloc(count, sizeof *candles);
    if (!candles)
        return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < count; ++i)
    {
        bool volume;
        status = UmiIbkrHistoricalCandleCopy(c, request, first + i, now, age, &candles[i], &volume);
        if (status != UMI_STATUS_OK)
        {
            free(candles);
            return status;
        }
    }
    status=UmiIbkrMarketBarScene(candles,count,"Historical capture | last bar may be unfinished",out);
    free(candles);
    return status;
}
