/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_resize_marquee.c
 *
 * PURPOSE:
 *   Verify constrained resize sessions and rectangular multi-selection on the
 *   semantic canvas.
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
    UmiWorkbenchDesignerResizeSession resize;
    UmiWorkbenchDesignerResizePolicy policy;
    UmiWorkbenchDesignerMarquee marquee;
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerCanvas canvas;
    const UmiWorkbenchLayoutNode *editor;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    policy = umi_workbench_designer_resize_policy_default();
    umi_workbench_designer_resize_init(&resize);
    TEST_REQUIRE_STATUS(umi_workbench_designer_resize_begin(
        &resize, "editor", UMI_WORKBENCH_DESIGNER_RESIZE_SOUTH_EAST,
        (UmiWorkbenchDesignerPoint){1000.0, 590.0},
        (UmiWorkbenchDesignerRect){280.0, 0.0, 1000.0, 590.0}, &policy));
    TEST_REQUIRE_STATUS(umi_workbench_designer_resize_update(
        &resize, (UmiWorkbenchDesignerPoint){1100.0, 650.0},
        (UmiWorkbenchDesignerRect){0.0, 0.0, 1600.0, 1000.0}));
    TEST_REQUIRE(resize.preview_bounds.width > resize.original_bounds.width);
    TEST_REQUIRE_STATUS(umi_workbench_designer_resize_commit(
        &resize, &document));
    editor = umi_workbench_layout_document_find_node(&document, "editor");
    TEST_REQUIRE(editor != NULL && editor->bounds.width > 1000);

    umi_workbench_designer_viewport_init(&viewport, 1280.0, 800.0);
    umi_workbench_designer_selection_init(&selection);
    umi_workbench_designer_canvas_init(&canvas);
    TEST_REQUIRE_STATUS(umi_workbench_designer_canvas_build(
        &canvas, &document, &viewport, &selection,
        (UmiWorkbenchDesignerRect){0.0, 0.0, 1280.0, 800.0}));
    umi_workbench_designer_marquee_init(&marquee);
    TEST_REQUIRE_STATUS(umi_workbench_designer_marquee_begin(
        &marquee, (UmiWorkbenchDesignerPoint){0.0, 580.0},
        UMI_WORKBENCH_DESIGNER_MARQUEE_INTERSECT, false));
    TEST_REQUIRE_STATUS(umi_workbench_designer_marquee_update(
        &marquee, (UmiWorkbenchDesignerPoint){1280.0, 800.0}));
    TEST_REQUIRE_STATUS(umi_workbench_designer_marquee_apply(
        &marquee, &canvas, &selection));
    TEST_REQUIRE(selection.count >= 2U);
    return 0;
}
