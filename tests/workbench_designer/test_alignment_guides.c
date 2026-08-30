/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_alignment_guides.c
 *
 * PURPOSE:
 *   Verify dynamic alignment guides are produced when a moving rectangle
 *   approaches another canvas edge or centre.
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
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerCanvas canvas;
    UmiWorkbenchDesignerAlignmentGuideModel guides;
    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_selection_init(&selection);
    umi_workbench_designer_viewport_init(&viewport, 1280.0, 800.0);
    umi_workbench_designer_canvas_init(&canvas);
    TEST_REQUIRE_STATUS(umi_workbench_designer_canvas_build(
        &canvas, &document, &viewport, &selection,
        (UmiWorkbenchDesignerRect){0.0, 0.0, 1280.0, 800.0}));
    umi_workbench_designer_alignment_guides_init(&guides, 8.0);
    TEST_REQUIRE_STATUS(umi_workbench_designer_alignment_guides_build(
        &guides, &canvas, "output",
        (UmiWorkbenchDesignerRect){780.0, 590.0, 500.0, 210.0}));
    TEST_REQUIRE(guides.count > 0U);
    TEST_REQUIRE(umi_workbench_designer_alignment_guide_at(&guides, 0U) != NULL);
    return 0;
}
