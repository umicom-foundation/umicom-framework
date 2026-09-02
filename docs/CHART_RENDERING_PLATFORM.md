<!--
Umicom Framework
File: docs/CHART_RENDERING_PLATFORM.md

PURPOSE:
Explain how applications build safe charts once and draw them through different
graphics adapters.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Chart Rendering Platform

The chart platform separates data, visual calculation and drawing. This is
important because a trading window, operational dashboard, report generator
and web application may show the same values through different graphics
systems. They should not each contain another copy of the scaling rules.

## The three layers

1. `umicom/chart/model.h` stores chart series and their values.
2. `umicom/chart/plot.h` maps time and values into logical pixel coordinates.
3. `umicom/chart/renderer.h` sends the resulting scene to a graphics adapter.

The middle result is an owned `UmiChartRenderScene`. A scene contains only
bounded value records: lines, filled or outlined rectangles, circles and short
text labels. It does not contain pointers to temporary strings or toolkit
objects. This makes a scene safe to clone and hand to a window that may outlive
the function that prepared it.

A scene can also record the logical width and height used when it was built.
The renderer scales a copied command into the current target size, allowing a
docked, floating or maximised chart to resize without changing its source data.

## Creating a candlestick scene

An application normally performs these steps:

1. validate or receive an array of `UmiChartCandle` values;
2. create a scene with enough capacity for its frame and candles;
3. initialise a light or dark `UmiChartPlotStyle` and apply user preferences;
4. calculate a viewport with `umi_chart_plot_viewport_from_candles`;
5. add the background and grid with `umi_chart_plot_add_frame`;
6. add candle wicks and bodies with `umi_chart_plot_add_candlesticks`;
7. give the finished scene to a frontend widget or export adapter; and
8. destroy the caller-owned scene when it is no longer needed.

Price values are never used directly as screen coordinates. The plot layer
maps the visible minimum and maximum into the available height and reverses the
vertical direction because screen positions grow downward while larger values
belong near the top of a chart.

The same layer can add connected lines, scatter markers and vertical bars. A
future chart type should add reusable scene commands here rather than drawing
directly in an application.

Indicator calculations remain independent of drawing. For example, a moving
average produced by `umicom/chart/indicator.h` is an ordinary point series and
can be passed to `umi_chart_plot_add_line_series`. This lets one calculated
indicator appear in a desktop chart, exported image or report without running
the calculation again in each frontend.

## Renderer contract

`UmiChartRenderer` is a small versioned callback table. A renderer declares
whether it supports lines, rectangles, circles and text. Before drawing begins,
the Framework checks the whole scene against those capabilities. An unsupported
command therefore cannot leave a half-rendered chart.

The renderer lifecycle is balanced:

- `begin` prepares and clips one frame;
- drawing callbacks receive validated value records; and
- `end` restores backend state even when a command reports an error.

The first native vector adapter is isolated under `chart/adapters`. Other
native, browser, image, print or accelerated adapters can implement the same
contract without changing chart data, plot builders or client applications.

## Desktop ownership and live updates

The GTK4 workstation chart surface clones the scene it receives. This avoids a
dangling pointer when an application builds a scene in temporary code and then
returns. Live data can build another scene and call
`umi_gtk4_ws_chart_surface_set_scene` on the UI thread. The widget clones the
replacement before discarding its previous scene, so an allocation failure
does not remove the chart already visible to the user.

When no scene is available, the widget displays a quiet themed frame rather
than invented market data. Thin applications remain responsible for choosing
the instrument, interval, data source and user theme; the Framework remains
responsible for safe reusable visual behaviour.

## Safety rules

- Check every status result while constructing a scene.
- Treat `UMI_STATUS_CAPACITY_EXCEEDED` as a request for a larger scene, not as
  permission to drop unknown commands silently.
- Reject non-finite values before projection.
- Validate every candle so its high and low contain its opening and closing
  values and its volume is non-negative.
- Keep adapter-owned drawing contexts alive for the complete render call.
- Update native widgets on their frontend event thread.
- Keep credentials, account identifiers and private order details out of text
  labels unless the application has applied its own disclosure policy.

These rules make the same chart pipeline suitable for market data, application
health, finance, education, scientific plots and future Umicom applications.
