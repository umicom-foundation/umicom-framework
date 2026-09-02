/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/plot.h
 *
 * PURPOSE:
 *   Convert chart values into reusable line, bar, scatter and candlestick scenes.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

/*
 * Plot builders perform all data-to-pixel calculations before a graphics
 * adapter is called.  This keeps scaling identical across desktop, browser,
 * exported document and image renderers.
 */

#ifndef UMICOM_CHART_PLOT_H
#define UMICOM_CHART_PLOT_H

#include "umicom/chart/candle.h"
#include "umicom/chart/render_scene.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_PLOT_MAX_GRID_LINES 64U

/* Describe the visible data range and the rectangle where it will be drawn. */
typedef struct UmiChartPlotViewport {
    UmiChartRenderRectangle area;
    int64_t start_ms;
    int64_t end_ms;
    double minimum_value;
    double maximum_value;
} UmiChartPlotViewport;

/* Keep chart colours and measurements together so applications can theme them. */
typedef struct UmiChartPlotStyle {
    UmiChartColor background_color;
    UmiChartColor border_color;
    UmiChartColor grid_color;
    UmiChartColor line_color;
    UmiChartColor positive_color;
    UmiChartColor negative_color;
    UmiChartColor marker_color;
    double grid_stroke_width;
    double series_stroke_width;
    double candle_gap_ratio;
    double marker_radius;
    size_t vertical_grid_lines;
    size_t horizontal_grid_lines;
} UmiChartPlotStyle;

/* Fill a style with accessible colours intended for a dark application theme. */
void umi_chart_plot_style_dark(UmiChartPlotStyle *style);

/* Fill a style with accessible colours intended for a light application theme. */
void umi_chart_plot_style_light(UmiChartPlotStyle *style);

/* Verify geometry, ranges, colours and style measurements before scene creation. */
UmiStatus umi_chart_plot_validate(
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style
);

/* Find the time and value range needed to display a candle collection. */
UmiStatus umi_chart_plot_viewport_from_candles(
    const UmiChartCandle *candles,
    size_t candle_count,
    UmiChartRenderRectangle area,
    double value_padding_ratio,
    UmiChartPlotViewport *out_viewport
);

/* Map a timestamp to a horizontal logical pixel position. */
UmiStatus umi_chart_plot_map_time(
    const UmiChartPlotViewport *viewport,
    int64_t time_ms,
    double *out_x
);

/* Map a value to an inverted vertical logical pixel position. */
UmiStatus umi_chart_plot_map_value(
    const UmiChartPlotViewport *viewport,
    double value,
    double *out_y
);

/* Add the themed background, plot border and evenly spaced grid lines. */
UmiStatus umi_chart_plot_add_frame(
    UmiChartRenderScene *scene,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style
);

/* Add a connected line plot from time/value points. */
UmiStatus umi_chart_plot_add_line_series(
    UmiChartRenderScene *scene,
    const UmiChartPoint *points,
    size_t point_count,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style
);

/* Add circular point markers for a scatter plot. */
UmiStatus umi_chart_plot_add_scatter_series(
    UmiChartRenderScene *scene,
    const UmiChartPoint *points,
    size_t point_count,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style
);

/* Add vertical bars using zero, or the nearest visible edge, as the baseline. */
UmiStatus umi_chart_plot_add_bar_series(
    UmiChartRenderScene *scene,
    const UmiChartPoint *points,
    size_t point_count,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style
);

/* Add correctly scaled candle wicks and bodies from validated market data. */
UmiStatus umi_chart_plot_add_candlesticks(
    UmiChartRenderScene *scene,
    const UmiChartCandle *candles,
    size_t candle_count,
    const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style
);

#ifdef __cplusplus
}
#endif

#endif
