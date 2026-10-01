/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_appearance/test_render.c
 * PURPOSE: Check independent colour and command expectations across defaults and custom drawing styles.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiChartDrawingSnapshot drawing = Drawing(UMI_CHART_DRAWING_RANGE);
    UmiChartPlotStyle theme; umi_chart_plot_style_dark(&theme);
    UmiChartPlotViewport viewport = {{0,0,100,100},100,200,0,100};
    UmiChartRenderScene *scene = NULL; OK(umi_chart_render_scene_create(4, &scene));
    UmiChartRenderCommand command; UmiChartDrawingResolvedAppearance resolved;
    if (strcmp(name, "defaults") == 0) {
        for (int i = 1; i <= 6; ++i) {
            drawing = Drawing((UmiChartDrawingKind)i);
            OK(UmiChartDrawingAppearanceResolve(&drawing, &theme, &resolved));
            NEAR(resolved.width, i == 1 || i == 6 ? 2.0 : 1.4);
            if (i == 2) NEAR(resolved.outline.green, theme.positive_color.green);
            else if (i == 3) NEAR(resolved.outline.red, theme.negative_color.red);
            else if (i == 5) { NEAR(resolved.outline.blue, 0.9); NEAR(resolved.fill.alpha, 0.16); }
            else { NEAR(resolved.outline.red, 0.98); NEAR(resolved.outline.green, 0.72); NEAR(resolved.fill.alpha, 0); }
        }
    } else if (strcmp(name, "range-fill") == 0 || strcmp(name, "line") == 0 || strcmp(name, "level") == 0) {
        if (strcmp(name, "line") == 0) drawing = Drawing(UMI_CHART_DRAWING_TREND);
        if (strcmp(name, "level") == 0) drawing = Drawing(UMI_CHART_DRAWING_SUPPORT);
        strcpy(drawing.style, "umi-drawing:1:33AAFF:25:30");
        OK(UmiChartDrawingRender(scene, &drawing, &viewport, &theme));
        CHECK(umi_chart_render_scene_count(scene) == (strcmp(name, "line") == 0 ? 1U : 2U));
        OK(umi_chart_render_scene_at(scene, 0, &command));
        NEAR(command.color.red, 0.2); NEAR(command.color.green, 170.0/255); NEAR(command.color.blue, 1);
        if (strcmp(name, "range-fill") == 0) {
            CHECK(command.kind == UMI_CHART_RENDER_FILL_RECTANGLE); NEAR(command.color.alpha, 0.3);
            OK(umi_chart_render_scene_at(scene, 1, &command)); CHECK(command.kind == UMI_CHART_RENDER_STROKE_RECTANGLE);
        } else CHECK(command.kind == UMI_CHART_RENDER_LINE);
        NEAR(command.stroke_width, 2.5); NEAR(command.color.alpha, 1);
        if (strcmp(name, "level") == 0) {
            OK(umi_chart_render_scene_at(scene, 1, &command)); CHECK(command.kind == UMI_CHART_RENDER_TEXT);
            CHECK(strstr(command.text, "support 25") != NULL); NEAR(command.color.red, 0.2);
        }
    } else if (strcmp(name, "zero-fill") == 0) {
        drawing = Drawing(UMI_CHART_DRAWING_LIQUIDITY_ZONE); strcpy(drawing.style, "umi-drawing:1:FFFFFF:80:00");
        OK(UmiChartDrawingRender(scene, &drawing, &viewport, &theme));
        CHECK(umi_chart_render_scene_count(scene) == 1);
        OK(umi_chart_render_scene_at(scene, 0, &command)); CHECK(command.kind == UMI_CHART_RENDER_STROKE_RECTANGLE); NEAR(command.stroke_width, 8);
    } else if (strcmp(name, "fallback") == 0) {
        const char *styles[] = {"old red", "umi-drawing:2:33AAFF:25:16", "umi-drawing:1:bad"};
        for (size_t i = 0; i < 3; ++i) {
            strcpy(drawing.style, styles[i]);
            OK(UmiChartDrawingAppearanceResolve(&drawing, &theme, &resolved));
            NEAR(resolved.outline.red, 0.98); NEAR(resolved.width, 1.4); NEAR(resolved.fill.alpha, 0);
            CHECK(strcmp(drawing.style, styles[i]) == 0);
        }
    } else if (strcmp(name, "capacity") == 0) {
        umi_chart_render_scene_destroy(scene); OK(umi_chart_render_scene_create(2, &scene));
        OK(umi_chart_render_scene_add_line(scene, (UmiChartRenderPoint){0,0}, (UmiChartRenderPoint){1,1}, (UmiChartColor){1,1,1,1}, 1));
        strcpy(drawing.style, "umi-drawing:1:33AAFF:25:30");
        CHECK(UmiChartDrawingRender(scene, &drawing, &viewport, &theme) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_chart_render_scene_count(scene) == 1);
        OK(umi_chart_render_scene_at(scene, 0, &command)); CHECK(command.start.x == 0 && command.end.x == 1);
    } else if (strcmp(name, "hidden") == 0) {
        strcpy(drawing.style, "umi-drawing:1:33AAFF:25:30"); drawing.visibility_flags = 1;
        OK(UmiChartDrawingRender(scene, &drawing, &viewport, &theme)); CHECK(umi_chart_render_scene_count(scene) == 0);
        drawing.visibility_flags = 0; drawing.time1 = 201; drawing.time2 = 300;
        OK(UmiChartDrawingRender(scene, &drawing, &viewport, &theme)); CHECK(umi_chart_render_scene_count(scene) == 0);
    } else return 2;
    umi_chart_render_scene_destroy(scene); return 0;
}
