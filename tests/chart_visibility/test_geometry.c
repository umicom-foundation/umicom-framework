/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_visibility/test_geometry.c
 * PURPOSE: Check that every supported hidden tool adds no drawing commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../chart_checkpoint/fixture.h"
#include "umicom/chart/drawing_tools.h"

/* A full scene proves that hidden annotations consume no render capacity. */
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiChartDrawingKind kind = UMI_CHART_DRAWING_TREND;
    int invalid = strcmp(argv[1], "invalid") == 0;
    if (!invalid) CHECK(UmiChartDrawingKindParse(argv[1], &kind) == UMI_STATUS_OK);
    UmiChartDrawingSnapshot drawing;
    CHECK(UmiChartDrawingInitialize("drawing", "NQ", kind,
        (UmiChartPoint){60000, 100}, (UmiChartPoint){120000, 110}, &drawing) == UMI_STATUS_OK);
    UmiChartPlotViewport viewport = {{0, 0, 800, 600}, 0, 180000, 90, 120};
    UmiChartDrawingGeometry geometry;
    CHECK(UmiChartDrawingProject(&drawing, &viewport, &geometry) == UMI_STATUS_OK && geometry.visible);
    UmiChartRenderScene *scene = NULL; UmiChartPlotStyle style; umi_chart_plot_style_dark(&style);
    CHECK(umi_chart_render_scene_create(1U, &scene) == UMI_STATUS_OK);
    CHECK(umi_chart_render_scene_add_line(scene, (UmiChartRenderPoint){0, 0},
        (UmiChartRenderPoint){800, 600}, (UmiChartColor){1, 1, 1, 1}, 1) == UMI_STATUS_OK);
    UmiChartRenderCommand before, after;
    CHECK(umi_chart_render_scene_at(scene, 0, &before) == UMI_STATUS_OK);
    drawing.visibility_flags = invalid ? 3U : UMI_CHART_DRAWING_VISIBILITY_HIDDEN;
    if (invalid) {
        memset(&geometry, 0x5a, sizeof geometry); UmiChartDrawingGeometry sentinel = geometry;
        CHECK(UmiChartDrawingProject(&drawing, &viewport, &geometry) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&sentinel, &geometry, sizeof geometry) == 0);
        CHECK(UmiChartDrawingRender(scene, &drawing, &viewport, &style) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        CHECK(UmiChartDrawingProject(&drawing, &viewport, &geometry) == UMI_STATUS_OK && !geometry.visible && geometry.kind == kind);
        CHECK(UmiChartDrawingRender(scene, &drawing, &viewport, &style) == UMI_STATUS_OK);
        drawing.visibility_flags = 0U;
        CHECK(UmiChartDrawingProject(&drawing, &viewport, &geometry) == UMI_STATUS_OK && geometry.visible);
    }
    CHECK(umi_chart_render_scene_count(scene) == 1U);
    CHECK(umi_chart_render_scene_at(scene, 0, &after) == UMI_STATUS_OK);
    CHECK(before.kind == after.kind && before.start.x == after.start.x && before.end.y == after.end.y && before.color.alpha == after.color.alpha);
    umi_chart_render_scene_destroy(scene); return 0;
}
