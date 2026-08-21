/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_drag_drop_guides.c
 *
 * PURPOSE:
 *   Verify pointer-independent dragging, target priority, guide activation and
 *   command planning.
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
    UmiWorkbenchDesignerDragSession drag;
    UmiWorkbenchDesignerDropTargetSet targets;
    UmiWorkbenchDesignerDockingGuideModel guides;
    UmiWorkbenchDesignerDragOperationPlan plan;
    const UmiWorkbenchDesignerDropTarget *target;
    const UmiWorkbenchDesignerDockingGuide *active;
    const UmiWorkbenchDesignerCanvasItem *editor;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_viewport_init(&viewport, 1280.0, 800.0);
    umi_workbench_designer_selection_init(&selection);
    umi_workbench_designer_canvas_init(&canvas);
    TEST_REQUIRE_STATUS(umi_workbench_designer_canvas_build(
        &canvas, &document, &viewport, &selection,
        (UmiWorkbenchDesignerRect){0.0, 0.0, 1280.0, 800.0}));
    editor = umi_workbench_designer_canvas_find(&canvas, "editor");
    TEST_REQUIRE(editor != NULL);
    umi_workbench_designer_drag_init(&drag);
    TEST_REQUIRE_STATUS(umi_workbench_designer_drag_begin(
        &drag, "output", (UmiWorkbenchDesignerPoint){300.0, 620.0},
        editor->screen_bounds, 1000U));
    TEST_REQUIRE_STATUS(umi_workbench_designer_drag_update(
        &drag, (UmiWorkbenchDesignerPoint){700.0, 300.0}, 1100U));
    umi_workbench_designer_drop_targets_init(&targets);
    TEST_REQUIRE_STATUS(umi_workbench_designer_drop_targets_build(
        &targets, &canvas, "output"));
    target = umi_workbench_designer_drop_target_hit_test(
        &targets, (UmiWorkbenchDesignerPoint){editor->screen_bounds.x + 4.0,
                                               editor->screen_bounds.y + 20.0});
    TEST_REQUIRE(target != NULL);
    umi_workbench_designer_docking_guides_init(&guides);
    TEST_REQUIRE_STATUS(umi_workbench_designer_docking_guides_build(
        &guides, &targets, target->target_node_id));
    TEST_REQUIRE(guides.count > 0U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_docking_guides_activate(
        &guides, guides.guides[0].guide_id));
    active = umi_workbench_designer_docking_guide_active(&guides);
    TEST_REQUIRE(active != NULL);
    TEST_REQUIRE_STATUS(umi_workbench_designer_drag_operation_plan(
        &drag, target, "command.drag", "user.sammy",
        document.version.revision, &plan));
    TEST_REQUIRE(plan.valid);
    return 0;
}
