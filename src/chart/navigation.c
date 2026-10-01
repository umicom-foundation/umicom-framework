/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/navigation.c
 * PURPOSE: Resolve bounded zoom, historical pan and chart hit coordinates.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/navigation.h"
#include <math.h>
#include <stdint.h>

UmiStatus UmiChartNavigationResolve(const UmiChartNavigation *navigation,
    const UmiChartCandle *candles, size_t count, UmiChartWindow *out_window)
{
    if (navigation == NULL || candles == NULL || out_window == NULL ||
        count == 0U || count > UMI_CHART_MAX_POINTS ||
        navigation->visible_bars > UMI_CHART_MAX_POINTS ||
        (navigation->pinned != 0 && navigation->pinned != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiChartTimeframeValid(navigation->interval_ms)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0; i < count; ++i) {
        if (umi_chart_candle_validate(&candles[i]) != UMI_STATUS_OK ||
            (i != 0U && candles[i].time_ms <= candles[i - 1U].time_ms))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    size_t shown = navigation->visible_bars;
    if (shown == 0U || shown > count) shown = count;
    size_t end = count;
    if (navigation->pinned) {
        end = 0U;
        while (end < count && candles[end].time_ms <= navigation->anchor_ms) ++end;
        /* Expired history clamps to the earliest complete retained window. */
        if (end < shown) end = shown;
    }
    *out_window = (UmiChartWindow){end - shown, shown};
    return UMI_STATUS_OK;
}
UmiStatus UmiChartNavigationZoom(UmiChartNavigation *navigation,
    const UmiChartCandle *candles, size_t count, int steps)
{
    UmiChartWindow window;
    UmiStatus status = UmiChartNavigationResolve(navigation, candles, count, &window);
    if (status != UMI_STATUS_OK) return status;
    if (steps < -32 || steps > 32) return UMI_STATUS_INVALID_ARGUMENT;
    if (steps == 0) return UMI_STATUS_OK;
    size_t shown = window.count, minimum = count < 4U ? count : 4U;
    if (shown < minimum) minimum = shown;
    for (int i = 0; i < (steps < 0 ? -steps : steps); ++i) {
        size_t change = shown / 4U;
        if (change == 0U) change = 1U;
        if (steps > 0) shown = shown > minimum + change ? shown - change : minimum;
        else shown = shown + change < count ? shown + change : count;
    }
    navigation->visible_bars = shown;
    return UMI_STATUS_OK;
}
UmiStatus UmiChartNavigationPan(UmiChartNavigation *navigation,
    const UmiChartCandle *candles, size_t count, int bars)
{
    UmiChartWindow window;
    UmiStatus status = UmiChartNavigationResolve(navigation, candles, count, &window);
    if (status != UMI_STATUS_OK) return status;
    if (bars == 0) return UMI_STATUS_OK;
    int64_t first = (int64_t)window.first + (int64_t)bars;
    if (first < 0) first = 0;
    if ((uint64_t)first > count - window.count) first = (int64_t)(count - window.count);
    size_t end = (size_t)first + window.count;
    navigation->visible_bars = window.count;
    navigation->anchor_ms = candles[end - 1U].time_ms;
    navigation->pinned = end != count;
    return UMI_STATUS_OK;
}
UmiStatus UmiChartPlotUnmap(const UmiChartPlotViewport *viewport,
    double x, double y, UmiChartPoint *out_point)
{
    UmiChartPlotStyle style;
    umi_chart_plot_style_dark(&style);
    if (viewport == NULL || out_point == NULL || !isfinite(x) || !isfinite(y) ||
        umi_chart_plot_validate(viewport, &style) != UMI_STATUS_OK ||
        x < viewport->area.x || x > viewport->area.x + viewport->area.width ||
        y < viewport->area.y || y > viewport->area.y + viewport->area.height)
        return UMI_STATUS_INVALID_ARGUMENT;
    long double ratio = ((long double)x - viewport->area.x) / viewport->area.width;
    long double time = viewport->start_ms + ratio *
        ((long double)viewport->end_ms - viewport->start_ms);
    long double value = viewport->maximum_value -
        ((long double)y - viewport->area.y) / viewport->area.height *
        ((long double)viewport->maximum_value - viewport->minimum_value);
    /* Strict limits avoid a rounded double representation of INT64_MAX being
     * converted out of range on platforms where long double equals double. */
    if (!isfinite(time) || time <= (long double)INT64_MIN || time >= (long double)INT64_MAX ||
        !isfinite((double)value)) return UMI_STATUS_INVALID_ARGUMENT;
    *out_point = (UmiChartPoint){(int64_t)time, (double)value};
    return UMI_STATUS_OK;
}
