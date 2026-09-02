/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_focus_ring.c
 *
 * PURPOSE:
 *   Verify keyboard focus order, explicit focus restoration and wrapped
 *   forward/backward navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerCanvas canvas;
    UmiWorkbenchDesignerFocusRing ring;
    const UmiWorkbenchDesignerFocusItem *item;
    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_selection_init(&selection);
    umi_workbench_designer_viewport_init(&viewport, 1280.0, 800.0);
    umi_workbench_designer_canvas_init(&canvas);
    TEST_REQUIRE_STATUS(umi_workbench_designer_canvas_build(
        &canvas, &document, &viewport, &selection,
        (UmiWorkbenchDesignerRect){0.0, 0.0, 1280.0, 800.0}));
    umi_workbench_designer_focus_ring_init(&ring);
    TEST_REQUIRE_STATUS(umi_workbench_designer_focus_ring_build(
        &ring, &canvas, false));
    TEST_REQUIRE(ring.count >= 4U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_focus_ring_set(&ring, "editor"));
    item = umi_workbench_designer_focus_ring_current(&ring);
    TEST_REQUIRE(item != NULL && strcmp(item->node_id, "editor") == 0);
    item = umi_workbench_designer_focus_ring_move(&ring, 1, true);
    TEST_REQUIRE(item != NULL && strcmp(item->node_id, "editor") != 0);
    item = umi_workbench_designer_focus_ring_move(&ring, -1, true);
    TEST_REQUIRE(item != NULL && strcmp(item->node_id, "editor") == 0);
    return 0;
}
