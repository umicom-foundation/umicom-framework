/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_viewport_history.c
 *
 * PURPOSE:
 *   Verify viewport back, forward, named restoration and forward-history
 *   truncation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerViewportHistory history;
    umi_workbench_designer_viewport_init(&viewport, 1280.0, 800.0);
    umi_workbench_designer_viewport_history_init(&history);
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_history_push(
        &history, &viewport, "home", "Home", 100U, true));
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_pan(
        &viewport, 100.0, 50.0));
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_history_push(
        &history, &viewport, "details", "Details", 200U, false));
    TEST_REQUIRE(umi_workbench_designer_viewport_history_can_back(&history));
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_history_back(
        &history, &viewport));
    TEST_REQUIRE(viewport.origin.x == 0.0 && viewport.origin.y == 0.0);
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_history_forward(
        &history, &viewport));
    TEST_REQUIRE(viewport.origin.x == -100.0 && viewport.origin.y == -50.0);
    TEST_REQUIRE_STATUS(umi_workbench_designer_viewport_history_restore(
        &history, "home", &viewport));
    TEST_REQUIRE(viewport.origin.x == 0.0);
    return 0;
}
