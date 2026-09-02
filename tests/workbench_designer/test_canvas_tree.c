/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_canvas_tree.c
 *
 * PURPOSE:
 *   Verify semantic layout projection into canvas items, hierarchy rows, hit
 *   testing and selected bounds.
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
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerCanvas canvas;
    UmiWorkbenchDesignerTree tree;
    const UmiWorkbenchDesignerCanvasItem *editor;
    const UmiWorkbenchDesignerCanvasItem *hit;
    const UmiWorkbenchDesignerTreeRow *row;
    UmiWorkbenchDesignerRect selection_bounds;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_viewport_init(&viewport, 1280.0, 800.0);
    umi_workbench_designer_selection_init(&selection);
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "editor", true));
    umi_workbench_designer_canvas_init(&canvas);
    TEST_REQUIRE_STATUS(umi_workbench_designer_canvas_build(
        &canvas, &document, &viewport, &selection,
        (UmiWorkbenchDesignerRect){0.0, 0.0, 1280.0, 800.0}));
    TEST_REQUIRE(canvas.count == document.node_count);
    editor = umi_workbench_designer_canvas_find(&canvas, "editor");
    TEST_REQUIRE(editor != NULL);
    TEST_REQUIRE(editor->selected);
    hit = umi_workbench_designer_canvas_hit_test(
        &canvas,
        (UmiWorkbenchDesignerPoint){editor->screen_bounds.x + 10.0,
                                    editor->screen_bounds.y + 10.0},
        false);
    TEST_REQUIRE(hit != NULL);
    TEST_REQUIRE(strcmp(hit->node_id, "editor") == 0);
    selection_bounds = umi_workbench_designer_canvas_selection_bounds(&canvas);
    TEST_REQUIRE(selection_bounds.width > 0.0);

    umi_workbench_designer_tree_init(&tree);
    TEST_REQUIRE_STATUS(umi_workbench_designer_tree_build(
        &tree, &document, &selection));
    row = umi_workbench_designer_tree_find(&tree, "terminal");
    TEST_REQUIRE(row != NULL);
    TEST_REQUIRE(row->depth >= 2U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_tree_set_expanded(
        &tree, "bottom", false));
    TEST_REQUIRE(!umi_workbench_designer_tree_is_expanded(&tree, "bottom"));
    return 0;
}
