/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/plot.c
 *
 * PURPOSE:
 *   Project numeric and market data into toolkit-neutral chart render scenes.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/plot.h"

#include <math.h>

/* Return one when a plot rectangle can contain visible drawing commands. */
static int area_is_valid(UmiChartRenderRectangle area)
{
    return isfinite(area.x) && isfinite(area.y) &&
           isfinite(area.width) && isfinite(area.height) &&
           area.width > 0.0 && area.height > 0.0;
}

/* Return one only when every theme colour channel is usable by renderers. */
static int color_is_valid(UmiChartColor color)
{
    return isfinite(color.red) && isfinite(color.green) &&
           isfinite(color.blue) && isfinite(color.alpha) &&
           color.red >= 0.0 && color.red <= 1.0 &&
           color.green >= 0.0 && color.green <= 1.0 &&
           color.blue >= 0.0 && color.blue <= 1.0 &&
           color.alpha >= 0.0 && color.alpha <= 1.0;
}

/* Clamp a value to a closed range so off-screen samples meet the plot edge. */
static double clamp_value(double value, double minimum, double maximum)
{
    /* Values below the visible range are drawn at its lower edge. */
    if (value < minimum) {
        return minimum;
    }

    /* Values above the visible range are drawn at its upper edge. */
    if (value > maximum) {
        return maximum;
    }

    return value;
}

/* Fill a style with accessible colours intended for a dark application theme. */
void umi_chart_plot_style_dark(UmiChartPlotStyle *style)
{
    /* A NULL output simply means there is no caller-owned style to initialise. */
    if (style == NULL) {
        return;
    }

    *style = (UmiChartPlotStyle){
        .background_color = {0.055, 0.075, 0.105, 1.0},
        .border_color = {0.30, 0.36, 0.44, 1.0},
        .grid_color = {0.32, 0.38, 0.46, 0.24},
        .line_color = {0.31, 0.70, 0.96, 1.0},
        .positive_color = {0.18, 0.76, 0.52, 1.0},
        .negative_color = {0.94, 0.32, 0.38, 1.0},
        .marker_color = {0.97, 0.75, 0.24, 1.0},
        .grid_stroke_width = 1.0,
        .series_stroke_width = 1.5,
        .candle_gap_ratio = 0.28,
        .marker_radius = 3.0,
        .vertical_grid_lines = 7U,
        .horizontal_grid_lines = 5U
    };
}

/* Fill a style with accessible colours intended for a light application theme. */
void umi_chart_plot_style_light(UmiChartPlotStyle *style)
{
    /* A NULL output simply means there is no caller-owned style to initialise. */
    if (style == NULL) {
        return;
    }

    *style = (UmiChartPlotStyle){
        .background_color = {0.98, 0.985, 0.99, 1.0},
        .border_color = {0.32, 0.37, 0.43, 1.0},
        .grid_color = {0.20, 0.25, 0.31, 0.16},
        .line_color = {0.04, 0.39, 0.78, 1.0},
        .positive_color = {0.02, 0.55, 0.32, 1.0},
        .negative_color = {0.82, 0.13, 0.20, 1.0},
        .marker_color = {0.82, 0.48, 0.02, 1.0},
        .grid_stroke_width = 1.0,
        .series_stroke_width = 1.5,
        .candle_gap_ratio = 0.28,
        .marker_radius = 3.0,
        .vertical_grid_lines = 7U,
        .horizontal_grid_lines = 5U
    };
}

/* Verify geometry, ranges, colours and style measurements before scene creation. */
UmiStatus umi_chart_plot_validate(
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style)
{
    /* Both records are required because each contributes rendering rules. */
    if (viewport == NULL || style == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* A plot needs positive geometry, ordered time and a non-empty value range. */
    if (!area_is_valid(viewport->area) ||
        viewport->end_ms < viewport->start_ms ||
        !isfinite(viewport->minimum_value) ||
        !isfinite(viewport->maximum_value) ||
        viewport->maximum_value <= viewport->minimum_value) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Every theme colour is checked centrally so all adapters receive safe values. */
    if (!color_is_valid(style->background_color) ||
        !color_is_valid(style->border_color) ||
        !color_is_valid(style->grid_color) ||
        !color_is_valid(style->line_color) ||
        !color_is_valid(style->positive_color) ||
        !color_is_valid(style->negative_color) ||
        !color_is_valid(style->marker_color)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Measurements and ratios must remain finite and visibly positive. */
    if (!isfinite(style->grid_stroke_width) ||
        !isfinite(style->series_stroke_width) ||
        !isfinite(style->candle_gap_ratio) ||
        !isfinite(style->marker_radius) ||
        style->grid_stroke_width <= 0.0 ||
        style->series_stroke_width <= 0.0 ||
        style->candle_gap_ratio < 0.0 ||
        style->candle_gap_ratio >= 1.0 ||
        style->marker_radius <= 0.0 ||
        style->vertical_grid_lines > UMI_CHART_PLOT_MAX_GRID_LINES ||
        style->horizontal_grid_lines > UMI_CHART_PLOT_MAX_GRID_LINES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/* Find the time and value range needed to display a candle collection. */
UmiStatus umi_chart_plot_viewport_from_candles(
    const UmiChartCandle *candles,
    size_t candle_count,
    UmiChartRenderRectangle area,
    double value_padding_ratio,
    UmiChartPlotViewport *out_viewport)
{
    double minimum_value;
    double maximum_value;
    double value_range;
    double padding;
    int64_t start_ms;
    int64_t end_ms;
    size_t index;

    /* A non-empty collection, visible area and output record are required. */
    if (candles == NULL || candle_count == 0U || out_viewport == NULL ||
        !area_is_valid(area) || !isfinite(value_padding_ratio) ||
        value_padding_ratio < 0.0 || value_padding_ratio > 1.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* The first validated candle provides safe starting bounds for the scan. */
    if (umi_chart_candle_validate(&candles[0]) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    minimum_value = candles[0].low;
    maximum_value = candles[0].high;
    start_ms = candles[0].time_ms;
    end_ms = candles[0].time_ms;

    /* Scan once so range calculation remains linear for large live datasets. */
    for (index = 1U; index < candle_count; ++index) {
        /* Reject malformed market data instead of drawing misleading geometry. */
        if (umi_chart_candle_validate(&candles[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Expand the value range whenever the current candle reaches a new extreme. */
        if (candles[index].low < minimum_value) {
            minimum_value = candles[index].low;
        }
        if (candles[index].high > maximum_value) {
            maximum_value = candles[index].high;
        }

        /* Expand the time range without requiring candles to arrive pre-sorted. */
        if (candles[index].time_ms < start_ms) {
            start_ms = candles[index].time_ms;
        }
        if (candles[index].time_ms > end_ms) {
            end_ms = candles[index].time_ms;
        }
    }

    value_range = maximum_value - minimum_value;
    /* Flat data still needs a visible value range, so add a small stable margin. */
    if (value_range == 0.0) {
        double reference = fabs(maximum_value);
        padding = reference > 1.0 ? reference * 0.01 : 1.0;
    } else {
        padding = value_range * value_padding_ratio;
    }

    /* Reject overflow before publishing an unusable viewport. */
    if (!isfinite(padding) ||
        !isfinite(minimum_value - padding) ||
        !isfinite(maximum_value + padding)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_viewport = (UmiChartPlotViewport){
        .area = area,
        .start_ms = start_ms,
        .end_ms = end_ms,
        .minimum_value = minimum_value - padding,
        .maximum_value = maximum_value + padding
    };
    return UMI_STATUS_OK;
}

/* Map a timestamp to a horizontal logical pixel position. */
UmiStatus umi_chart_plot_map_time(
    const UmiChartPlotViewport *viewport,
    int64_t time_ms,
    double *out_x)
{
    double start;
    double end;
    double clamped_time;

    /* The mapping requires valid geometry, ordered time and an output location. */
    if (viewport == NULL || out_x == NULL ||
        !area_is_valid(viewport->area) ||
        viewport->end_ms < viewport->start_ms) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    start = (double)viewport->start_ms;
    end = (double)viewport->end_ms;
    /* A single timestamp is centred because there is no horizontal span to divide. */
    if (end == start) {
        *out_x = viewport->area.x + (viewport->area.width * 0.5);
        return UMI_STATUS_OK;
    }

    clamped_time = clamp_value((double)time_ms, start, end);
    *out_x = viewport->area.x +
             (((clamped_time - start) / (end - start)) * viewport->area.width);
    return UMI_STATUS_OK;
}

/* Map a value to an inverted vertical logical pixel position. */
UmiStatus umi_chart_plot_map_value(
    const UmiChartPlotViewport *viewport,
    double value,
    double *out_y)
{
    double clamped;
    double ratio;

    /* A visible finite range is required before division and pixel projection. */
    if (viewport == NULL || out_y == NULL || !area_is_valid(viewport->area) ||
        !isfinite(value) || !isfinite(viewport->minimum_value) ||
        !isfinite(viewport->maximum_value) ||
        viewport->maximum_value <= viewport->minimum_value) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    clamped = clamp_value(
        value,
        viewport->minimum_value,
        viewport->maximum_value);
    ratio = (clamped - viewport->minimum_value) /
            (viewport->maximum_value - viewport->minimum_value);

    /* Screen coordinates grow downward, so larger values use smaller y positions. */
    *out_y = viewport->area.y + ((1.0 - ratio) * viewport->area.height);
    return UMI_STATUS_OK;
}

/* Add the themed background, plot border and evenly spaced grid lines. */
UmiStatus umi_chart_plot_add_frame(
    UmiChartRenderScene *scene,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style)
{
    UmiStatus status;
    size_t index;

    /* Validate the scene separately because plot validation cannot inspect it. */
    if (scene == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_chart_plot_validate(viewport, style);
    /* Do not add a partial frame when its configuration is invalid. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_chart_render_scene_add_filled_rectangle(
        scene,
        viewport->area,
        style->background_color);
    /* Capacity errors are returned immediately so the caller can allocate a larger scene. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Add evenly spaced vertical grid lines inside, rather than over, the border. */
    for (index = 1U; index <= style->vertical_grid_lines; ++index) {
        double x = viewport->area.x +
                   (viewport->area.width * (double)index /
                    (double)(style->vertical_grid_lines + 1U));
        status = umi_chart_render_scene_add_line(
            scene,
            (UmiChartRenderPoint){x, viewport->area.y},
            (UmiChartRenderPoint){x, viewport->area.y + viewport->area.height},
            style->grid_color,
            style->grid_stroke_width);
        /* Stop before later lines if the bounded scene becomes full. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Add evenly spaced horizontal grid lines for quick value comparison. */
    for (index = 1U; index <= style->horizontal_grid_lines; ++index) {
        double y = viewport->area.y +
                   (viewport->area.height * (double)index /
                    (double)(style->horizontal_grid_lines + 1U));
        status = umi_chart_render_scene_add_line(
            scene,
            (UmiChartRenderPoint){viewport->area.x, y},
            (UmiChartRenderPoint){viewport->area.x + viewport->area.width, y},
            style->grid_color,
            style->grid_stroke_width);
        /* Stop before later lines if the bounded scene becomes full. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    return umi_chart_render_scene_add_stroked_rectangle(
        scene,
        viewport->area,
        style->border_color,
        style->grid_stroke_width);
}

/* Add a connected line plot from time/value points. */
UmiStatus umi_chart_plot_add_line_series(
    UmiChartRenderScene *scene,
    const UmiChartPoint *points,
    size_t point_count,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style)
{
    UmiChartRenderPoint previous = {0};
    UmiStatus status;
    size_t index;

    /* At least one point and a valid destination are needed for a visible series. */
    if (scene == NULL || points == NULL || point_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_chart_plot_validate(viewport, style);
    /* Do not project points with an invalid range or style. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Project each point once and connect it to the preceding point. */
    for (index = 0U; index < point_count; ++index) {
        UmiChartRenderPoint current;

        /* Non-finite values are rejected because they cannot become safe coordinates. */
        if (!isfinite(points[index].value)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = umi_chart_plot_map_time(viewport, points[index].time_ms, &current.x);
        /* Preserve a projection failure rather than drawing guessed coordinates. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = umi_chart_plot_map_value(viewport, points[index].value, &current.y);
        /* Preserve a projection failure rather than drawing guessed coordinates. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        /* The first point has no predecessor; display it as a marker. */
        if (index == 0U && point_count == 1U) {
            return umi_chart_render_scene_add_circle(
                scene,
                current,
                style->marker_radius,
                style->line_color);
        }

        /* Every point after the first creates one connected line segment. */
        if (index > 0U) {
            status = umi_chart_render_scene_add_line(
                scene,
                previous,
                current,
                style->line_color,
                style->series_stroke_width);
            /* Stop if the bounded scene cannot accept another segment. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
        }
        previous = current;
    }

    return UMI_STATUS_OK;
}

/* Add circular point markers for a scatter plot. */
UmiStatus umi_chart_plot_add_scatter_series(
    UmiChartRenderScene *scene,
    const UmiChartPoint *points,
    size_t point_count,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style)
{
    UmiStatus status;
    size_t index;

    /* At least one point and a valid destination are needed for a scatter plot. */
    if (scene == NULL || points == NULL || point_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_chart_plot_validate(viewport, style);
    /* Do not project points with an invalid range or style. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Each data item becomes one independently selectable circular marker. */
    for (index = 0U; index < point_count; ++index) {
        UmiChartRenderPoint centre;

        /* Non-finite values are rejected because they cannot become safe coordinates. */
        if (!isfinite(points[index].value)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = umi_chart_plot_map_time(viewport, points[index].time_ms, &centre.x);
        /* Preserve any mapping failure before adding a partial marker. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = umi_chart_plot_map_value(viewport, points[index].value, &centre.y);
        /* Preserve any mapping failure before adding a partial marker. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = umi_chart_render_scene_add_circle(
            scene,
            centre,
            style->marker_radius,
            style->marker_color);
        /* Stop if the scene's documented capacity has been reached. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    return UMI_STATUS_OK;
}

/* Add vertical bars using zero, or the nearest visible edge, as the baseline. */
UmiStatus umi_chart_plot_add_bar_series(
    UmiChartRenderScene *scene,
    const UmiChartPoint *points,
    size_t point_count,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style)
{
    double baseline_value;
    double baseline_y;
    double bar_width;
    double minimum_bar_width;
    double minimum_bar_height;
    UmiStatus status;
    size_t index;

    /* At least one point and a valid destination are needed for a bar plot. */
    if (scene == NULL || points == NULL || point_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_chart_plot_validate(viewport, style);
    /* Do not calculate bar measurements with an invalid plot. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    baseline_value = clamp_value(
        0.0,
        viewport->minimum_value,
        viewport->maximum_value);
    status = umi_chart_plot_map_value(viewport, baseline_value, &baseline_y);
    /* A failed baseline mapping prevents every bar from being positioned safely. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    minimum_bar_width = viewport->area.width < 1.0
                            ? viewport->area.width
                            : 1.0;
    minimum_bar_height = viewport->area.height < 1.0
                             ? viewport->area.height
                             : 1.0;
    bar_width = (viewport->area.width / (double)point_count) * 0.65;
    /* Keep dense charts visible without allowing unusually wide sparse bars. */
    bar_width = clamp_value(
        bar_width,
        minimum_bar_width,
        viewport->area.width < 48.0 ? viewport->area.width : 48.0);

    /* Each point becomes a rectangle extending to the shared baseline. */
    for (index = 0U; index < point_count; ++index) {
        UmiChartRenderRectangle rectangle;
        UmiChartColor color;
        double x;
        double value_y;

        /* Non-finite values are rejected because they cannot become safe rectangles. */
        if (!isfinite(points[index].value)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = umi_chart_plot_map_time(viewport, points[index].time_ms, &x);
        /* Preserve a mapping failure before adding part of the series. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        /* Keep the complete first and last bar inside the visible plot rectangle. */
        x = clamp_value(
            x,
            viewport->area.x + (bar_width * 0.5),
            viewport->area.x + viewport->area.width - (bar_width * 0.5));
        status = umi_chart_plot_map_value(viewport, points[index].value, &value_y);
        /* Preserve a mapping failure before adding part of the series. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        rectangle.x = x - (bar_width * 0.5);
        rectangle.y = value_y < baseline_y ? value_y : baseline_y;
        rectangle.width = bar_width;
        rectangle.height = fabs(baseline_y - value_y);
        /* A zero-value bar remains visible as a one-pixel baseline mark. */
        if (rectangle.height < minimum_bar_height) {
            rectangle.height = minimum_bar_height;
            rectangle.y -= minimum_bar_height * 0.5;
        }
        /* Use the true zero sign for colour even when zero is outside the viewport. */
        color = points[index].value >= 0.0
                    ? style->positive_color
                    : style->negative_color;
        /* Keep a one-pixel baseline bar inside the visible vertical plot bounds. */
        rectangle.y = clamp_value(
            rectangle.y,
            viewport->area.y,
            viewport->area.y + viewport->area.height - rectangle.height);
        status = umi_chart_render_scene_add_filled_rectangle(scene, rectangle, color);
        /* Stop if the bounded scene cannot accept the complete series. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    return UMI_STATUS_OK;
}

/* Add correctly scaled candle wicks and bodies from validated market data. */
UmiStatus umi_chart_plot_add_candlesticks(
    UmiChartRenderScene *scene,
    const UmiChartCandle *candles,
    size_t candle_count,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style)
{
    double candle_width;
    double minimum_candle_width;
    double minimum_candle_height;
    UmiStatus status;
    size_t index;

    /* A candle collection and scene are required for a market plot. */
    if (scene == NULL || candles == NULL || candle_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_chart_plot_validate(viewport, style);
    /* Do not calculate widths or coordinates for an invalid plot. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    minimum_candle_width = viewport->area.width < 1.0
                               ? viewport->area.width
                               : 1.0;
    minimum_candle_height = viewport->area.height < 1.0
                                ? viewport->area.height
                                : 1.0;
    candle_width = (viewport->area.width / (double)candle_count) *
                   (1.0 - style->candle_gap_ratio);
    /* Keep large sparse candles readable and dense candles at least one pixel wide. */
    candle_width = clamp_value(
        candle_width,
        minimum_candle_width,
        viewport->area.width < 36.0 ? viewport->area.width : 36.0);

    /* Each valid candle becomes one wick followed by one coloured body. */
    for (index = 0U; index < candle_count; ++index) {
        UmiChartRenderRectangle body;
        UmiChartColor color;
        double x;
        double high_y;
        double low_y;
        double open_y;
        double close_y;

        /* Malformed market data is rejected instead of producing misleading candles. */
        if (umi_chart_candle_validate(&candles[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = umi_chart_plot_map_time(viewport, candles[index].time_ms, &x);
        /* Preserve the first projection failure for the caller. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        /* Keep the complete first and last candle body inside the plot rectangle. */
        x = clamp_value(
            x,
            viewport->area.x + (candle_width * 0.5),
            viewport->area.x + viewport->area.width - (candle_width * 0.5));
        status = umi_chart_plot_map_value(viewport, candles[index].high, &high_y);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = umi_chart_plot_map_value(viewport, candles[index].low, &low_y);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = umi_chart_plot_map_value(viewport, candles[index].open, &open_y);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = umi_chart_plot_map_value(viewport, candles[index].close, &close_y);
        if (status != UMI_STATUS_OK) {
            return status;
        }

        color = umi_chart_candle_is_up(&candles[index])
                    ? style->positive_color
                    : style->negative_color;
        status = umi_chart_render_scene_add_line(
            scene,
            (UmiChartRenderPoint){x, high_y},
            (UmiChartRenderPoint){x, low_y},
            color,
            style->series_stroke_width);
        /* Do not add a body if the matching wick could not be stored. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        body.x = x - (candle_width * 0.5);
        body.y = open_y < close_y ? open_y : close_y;
        body.width = candle_width;
        body.height = fabs(close_y - open_y);
        /* A candle with equal open and close remains visible as a one-pixel body. */
        if (body.height < minimum_candle_height) {
            body.height = minimum_candle_height;
            body.y -= minimum_candle_height * 0.5;
        }
        /* Keep a one-pixel flat body inside the visible vertical plot bounds. */
        body.y = clamp_value(
            body.y,
            viewport->area.y,
            viewport->area.y + viewport->area.height - body.height);
        status = umi_chart_render_scene_add_filled_rectangle(scene, body, color);
        /* Stop if the scene cannot hold the complete candle collection. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    return UMI_STATUS_OK;
}
