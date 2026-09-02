/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_canvas_accessibility.c
 *
 * PURPOSE:
 *   Verify semantic canvas nodes produce accessible roles, focus, selection and
 *   action metadata.
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
    UmiWorkbenchDesignerTree tree;
    UmiWorkbenchDesignerFocusRing focus;
    UmiWorkbenchDesignerAccessibilityModel accessibility;
    const UmiWorkbenchDesignerAccessibleItem *editor;
    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_selection_init(&selection);
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "editor", true));
    umi_workbench_designer_viewport_init(&viewport, 1280.0, 800.0);
    umi_workbench_designer_canvas_init(&canvas);
    TEST_REQUIRE_STATUS(umi_workbench_designer_canvas_build(
        &canvas, &document, &viewport, &selection,
        (UmiWorkbenchDesignerRect){0.0, 0.0, 1280.0, 800.0}));
    umi_workbench_designer_tree_init(&tree);
    TEST_REQUIRE_STATUS(umi_workbench_designer_tree_build(
        &tree, &document, &selection));
    umi_workbench_designer_focus_ring_init(&focus);
    TEST_REQUIRE_STATUS(umi_workbench_designer_focus_ring_build(
        &focus, &canvas, false));
    TEST_REQUIRE_STATUS(umi_workbench_designer_focus_ring_set(&focus, "editor"));
    umi_workbench_designer_accessibility_init(&accessibility);
    TEST_REQUIRE_STATUS(umi_workbench_designer_accessibility_build(
        &accessibility, &canvas, &tree, &focus, false));
    editor = umi_workbench_designer_accessibility_find(&accessibility, "editor");
    TEST_REQUIRE(editor != NULL && editor->focused && editor->selected);
    TEST_REQUIRE(editor->role == UMI_WORKBENCH_DESIGNER_ACCESSIBLE_EDITOR);
    return 0;
}
