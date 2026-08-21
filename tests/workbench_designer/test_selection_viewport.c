/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_selection_viewport.c
 *
 * PURPOSE:
 *   Verify deterministic multi-selection, primary identity and reversible
 *   world/screen coordinate conversion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerPoint world = {150.0, 75.0};
    UmiWorkbenchDesignerPoint screen;
    UmiWorkbenchDesignerPoint restored;
    UmiWorkbenchDesignerRect fit = {100.0, 50.0, 400.0, 200.0};

    umi_workbench_designer_selection_init(&selection);
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "editor", true));
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "terminal", false));
    TEST_REQUIRE(selection.count == 2U);
    TEST_REQUIRE(strcmp(
        umi_workbench_designer_selection_primary(&selection), "editor") == 0);
    TEST_REQUIRE(umi_workbench_designer_selection_contains(
        &selection, "terminal"));
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_toggle(
        &selection, "terminal", false));
    TEST_REQUIRE(selection.count == 1U);

    umi_workbench_designer_viewport_init(&viewport, 1000.0, 700.0);
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_pan(
        &viewport, 40.0, 20.0));
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_set_zoom(
        &viewport, 1.5, (UmiWorkbenchDesignerPoint){500.0, 350.0}));
    screen = umi_workbench_designer_viewport_world_to_screen(&viewport, world);
    restored = umi_workbench_designer_viewport_screen_to_world(&viewport, screen);
    TEST_REQUIRE(restored.x > world.x - 0.0001 &&
                 restored.x < world.x + 0.0001);
    TEST_REQUIRE(restored.y > world.y - 0.0001 &&
                 restored.y < world.y + 0.0001);
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_fit_rect(
        &viewport, &fit, 20.0));
    TEST_REQUIRE(viewport.zoom > 0.0);
    return 0;
}
