/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_preview_minimap_ruler.c
 *
 * PURPOSE:
 *   Verify immutable preview, minimap and ruler projections at non-default zoom.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerCanvas canvas;
    UmiWorkbenchDesignerMinimap minimap;
    UmiWorkbenchDesignerRuler ruler;
    UmiWorkbenchDesignerBrowserPreview preview;
    UmiWorkbenchDesignerPoint world;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_viewport_init(&viewport, 1280.0, 800.0);
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_set_zoom(
        &viewport, 1.25, (UmiWorkbenchDesignerPoint){640.0, 400.0}));
    umi_workbench_designer_selection_init(&selection);
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "editor", true));
    umi_workbench_designer_canvas_init(&canvas);
    TEST_REQUIRE_STATUS(umi_workbench_designer_canvas_build(
        &canvas, &document, &viewport, &selection,
        (UmiWorkbenchDesignerRect){0.0, 0.0, 1280.0, 800.0}));
    umi_workbench_designer_minimap_init(&minimap);
    TEST_REQUIRE_STATUS(umi_workbench_designer_minimap_build(
        &minimap, &canvas, &viewport,
        (UmiWorkbenchDesignerSize){240.0, 150.0}));
    TEST_REQUIRE(minimap.count > 0U);
    world = umi_workbench_designer_minimap_world_point(
        &minimap, (UmiWorkbenchDesignerPoint){10.0, 10.0});
    TEST_REQUIRE(world.x >= minimap.content_bounds.x);
    umi_workbench_designer_ruler_init(
        &ruler, UMI_WORKBENCH_DESIGNER_RULER_HORIZONTAL);
    TEST_REQUIRE_STATUS(umi_workbench_designer_ruler_build(
        &ruler, &viewport, 1280.0, 40.0));
    TEST_REQUIRE(ruler.count > 5U);
    umi_workbench_designer_browser_preview_init(&preview);
    TEST_REQUIRE_STATUS(umi_workbench_designer_browser_preview_build(
        &preview, &document, (UmiWorkbenchDesignerSize){260.0, 160.0}));
    TEST_REQUIRE(preview.available);
    TEST_REQUIRE(preview.panel_count >= 4U);
    return 0;
}
