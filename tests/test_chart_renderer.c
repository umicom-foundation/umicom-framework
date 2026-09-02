/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_chart_renderer.c
 *
 * PURPOSE:
 *   Verify bounded chart scenes, price projection and renderer dispatch.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/plot.h"
#include "umicom/chart/renderer.h"

#include <assert.h>
#include <math.h>

/* Record callback counts without depending on a desktop or graphics library. */
typedef struct RecordingRenderer {
    size_t begin_count;
    size_t line_count;
    size_t fill_rectangle_count;
    size_t stroke_rectangle_count;
    size_t circle_count;
    size_t text_count;
    size_t end_count;
} RecordingRenderer;

/* Record the beginning of one complete frame. */
static UmiStatus record_begin(void *context, double width, double height)
{
    RecordingRenderer *recording = (RecordingRenderer *)context;
    assert(recording != NULL);
    assert(width == 200.0);
    assert(height == 100.0);
    ++recording->begin_count;
    return UMI_STATUS_OK;
}

/* Record one line while checking the renderer received usable values. */
static UmiStatus record_line(
    void *context,
    UmiChartRenderPoint start,
    UmiChartRenderPoint end,
    UmiChartColor color,
    double stroke_width)
{
    RecordingRenderer *recording = (RecordingRenderer *)context;
    assert(recording != NULL);
    assert(start.x >= 0.0 && end.x >= 0.0);
    assert(color.alpha >= 0.0 && color.alpha <= 1.0);
    assert(stroke_width > 0.0);
    ++recording->line_count;
    return UMI_STATUS_OK;
}

/* Record one filled rectangle such as a background or candle body. */
static UmiStatus record_fill_rectangle(
    void *context,
    UmiChartRenderRectangle rectangle,
    UmiChartColor color)
{
    RecordingRenderer *recording = (RecordingRenderer *)context;
    assert(recording != NULL);
    assert(rectangle.width > 0.0 && rectangle.height > 0.0);
    assert(color.alpha >= 0.0 && color.alpha <= 1.0);
    ++recording->fill_rectangle_count;
    return UMI_STATUS_OK;
}

/* Record one rectangle outline such as the plot border. */
static UmiStatus record_stroke_rectangle(
    void *context,
    UmiChartRenderRectangle rectangle,
    UmiChartColor color,
    double stroke_width)
{
    RecordingRenderer *recording = (RecordingRenderer *)context;
    assert(recording != NULL);
    assert(rectangle.width > 0.0 && rectangle.height > 0.0);
    assert(color.alpha >= 0.0 && color.alpha <= 1.0);
    assert(stroke_width > 0.0);
    ++recording->stroke_rectangle_count;
    return UMI_STATUS_OK;
}

/* Record one circular data marker. */
static UmiStatus record_circle(
    void *context,
    UmiChartRenderPoint centre,
    double radius,
    UmiChartColor color)
{
    RecordingRenderer *recording = (RecordingRenderer *)context;
    assert(recording != NULL);
    assert(centre.x >= 0.0 && centre.y >= 0.0);
    assert(radius > 0.0);
    assert(color.alpha >= 0.0 && color.alpha <= 1.0);
    ++recording->circle_count;
    return UMI_STATUS_OK;
}

/* Record one bounded text label. */
static UmiStatus record_text(
    void *context,
    UmiChartRenderPoint position,
    const char *text,
    UmiChartColor color)
{
    RecordingRenderer *recording = (RecordingRenderer *)context;
    assert(recording != NULL);
    assert(position.x >= 0.0 && position.y >= 0.0);
    assert(text != NULL && text[0] != '\0');
    assert(color.alpha >= 0.0 && color.alpha <= 1.0);
    ++recording->text_count;
    return UMI_STATUS_OK;
}

/* Record the balanced end of a complete frame. */
static UmiStatus record_end(void *context)
{
    RecordingRenderer *recording = (RecordingRenderer *)context;
    assert(recording != NULL);
    ++recording->end_count;
    return UMI_STATUS_OK;
}

/* Exercise the public chart pipeline without opening any graphical window. */
int main(void)
{
    const UmiChartCandle candles[] = {
        {1000, 100.0, 110.0, 95.0, 105.0, 1200.0},
        {2000, 105.0, 115.0, 100.0, 102.0, 950.0}
    };
    const UmiChartPoint points[] = {
        {1000, 97.0},
        {1500, 105.0},
        {2000, 112.0}
    };
    UmiChartCandle invalid_candle = candles[0];
    UmiChartRenderScene *scene = NULL;
    UmiChartRenderScene *copy = NULL;
    UmiChartPlotViewport viewport;
    UmiChartPlotStyle style;
    UmiChartRenderCommand wick;
    UmiChartRenderCommand body;
    RecordingRenderer recording = {0};
    UmiChartRenderer renderer = {
        .struct_size = (uint32_t)sizeof(UmiChartRenderer),
        .api_version = UMI_CHART_RENDERER_API_VERSION,
        .capabilities = UMI_CHART_RENDERER_CAPABILITY_LINE |
                        UMI_CHART_RENDERER_CAPABILITY_RECTANGLE |
                        UMI_CHART_RENDERER_CAPABILITY_CIRCLE |
                        UMI_CHART_RENDERER_CAPABILITY_TEXT,
        .context = &recording,
        .operations = {
            .begin = record_begin,
            .draw_line = record_line,
            .fill_rectangle = record_fill_rectangle,
            .stroke_rectangle = record_stroke_rectangle,
            .fill_circle = record_circle,
            .draw_text = record_text,
            .end = record_end
        }
    };

    assert(umi_chart_render_scene_create(64U, &scene) == UMI_STATUS_OK);
    assert(scene != NULL);
    assert(umi_chart_render_scene_capacity(scene) == 64U);
    assert(umi_chart_render_scene_set_coordinate_size(
               scene,
               200.0,
               100.0) == UMI_STATUS_OK);

    umi_chart_plot_style_dark(&style);
    assert(umi_chart_plot_viewport_from_candles(
               candles,
               2U,
               (UmiChartRenderRectangle){0.0, 0.0, 200.0, 100.0},
               0.0,
               &viewport) == UMI_STATUS_OK);
    assert(viewport.start_ms == 1000);
    assert(viewport.end_ms == 2000);
    assert(viewport.minimum_value == 95.0);
    assert(viewport.maximum_value == 115.0);

    assert(umi_chart_plot_add_frame(scene, &viewport, &style) == UMI_STATUS_OK);
    assert(umi_chart_plot_add_candlesticks(
               scene,
               candles,
               2U,
               &viewport,
               &style) == UMI_STATUS_OK);
    assert(umi_chart_render_scene_count(scene) == 18U);

    /* The first candle wick follows fourteen frame commands. */
    assert(umi_chart_render_scene_at(scene, 14U, &wick) == UMI_STATUS_OK);
    assert(wick.kind == UMI_CHART_RENDER_LINE);
    assert(wick.start.y < wick.end.y);

    /* The candle body is scaled rather than using raw financial values as pixels. */
    assert(umi_chart_render_scene_at(scene, 15U, &body) == UMI_STATUS_OK);
    assert(body.kind == UMI_CHART_RENDER_FILL_RECTANGLE);
    assert(body.rectangle.y >= 0.0);
    assert(body.rectangle.y + body.rectangle.height <= 100.0);

    assert(umi_chart_render_scene_clone(scene, &copy) == UMI_STATUS_OK);
    assert(copy != NULL);
    assert(umi_chart_render_scene_count(copy) == umi_chart_render_scene_count(scene));

    assert(umi_chart_renderer_validate(&renderer) == UMI_STATUS_OK);
    assert(umi_chart_renderer_render_scene(
               &renderer,
               copy,
               200.0,
               100.0) == UMI_STATUS_OK);
    assert(recording.begin_count == 1U);
    assert(recording.line_count == 14U);
    assert(recording.fill_rectangle_count == 3U);
    assert(recording.stroke_rectangle_count == 1U);
    assert(recording.end_count == 1U);

    /* Clear keeps the responsive coordinate space while allowing another plot family. */
    umi_chart_render_scene_clear(copy);
    assert(umi_chart_plot_add_line_series(
               copy,
               points,
               3U,
               &viewport,
               &style) == UMI_STATUS_OK);
    assert(umi_chart_plot_add_scatter_series(
               copy,
               points,
               3U,
               &viewport,
               &style) == UMI_STATUS_OK);
    assert(umi_chart_plot_add_bar_series(
               copy,
               points,
               3U,
               &viewport,
               &style) == UMI_STATUS_OK);
    assert(umi_chart_render_scene_count(copy) == 8U);

    invalid_candle.high = NAN;
    /* Non-finite market values are rejected before they become drawing coordinates. */
    assert(umi_chart_candle_validate(&invalid_candle) ==
           UMI_STATUS_INVALID_ARGUMENT);

    /* Restore the candle scene for the capability-preflight regression below. */
    umi_chart_render_scene_destroy(copy);
    copy = NULL;
    assert(umi_chart_render_scene_clone(scene, &copy) == UMI_STATUS_OK);
    assert(umi_chart_render_scene_add_circle(
               copy,
               (UmiChartRenderPoint){10.0, 10.0},
               2.0,
               style.marker_color) == UMI_STATUS_OK);
    renderer.capabilities &= ~((uint32_t)UMI_CHART_RENDERER_CAPABILITY_CIRCLE);
    /* Capability preflight rejects the scene before beginning a partial frame. */
    assert(umi_chart_renderer_render_scene(
               &renderer,
               copy,
               200.0,
               100.0) == UMI_STATUS_NOT_IMPLEMENTED);
    assert(recording.begin_count == 1U);

    umi_chart_render_scene_destroy(copy);
    umi_chart_render_scene_destroy(scene);
    return 0;
}
