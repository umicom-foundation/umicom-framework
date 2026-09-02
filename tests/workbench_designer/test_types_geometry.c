/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_types_geometry.c
 *
 * PURPOSE:
 *   Verify stable text mappings, geometry validation, intersections and clamping
 *   used by every frontend adapter.
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
    UmiWorkbenchDesignerRect outer = {0.0, 0.0, 100.0, 80.0};
    UmiWorkbenchDesignerRect inner = {20.0, 10.0, 30.0, 40.0};
    UmiWorkbenchDesignerRect disjoint = {130.0, 90.0, 20.0, 20.0};
    UmiWorkbenchDesignerRect intersection;
    UmiWorkbenchDesignerPoint point = {25.0, 20.0};

    TEST_REQUIRE(strcmp(
        umi_workbench_designer_state_text(
            UMI_WORKBENCH_DESIGNER_STATE_RUNNING), "running") == 0);
    TEST_REQUIRE(strcmp(
        umi_workbench_designer_mode_text(
            UMI_WORKBENCH_DESIGNER_MODE_DESIGN), "design") == 0);
    TEST_REQUIRE(strcmp(
        umi_workbench_designer_tool_text(
            UMI_WORKBENCH_DESIGNER_TOOL_SPLIT_HORIZONTAL),
        "split-horizontal") == 0);
    TEST_REQUIRE(strcmp(
        umi_workbench_designer_drop_zone_text(
            UMI_WORKBENCH_DESIGNER_DROP_TAB), "tab") == 0);
    TEST_REQUIRE(umi_workbench_designer_rect_is_valid(&outer));
    TEST_REQUIRE(umi_workbench_designer_rect_contains_point(&outer, point));
    TEST_REQUIRE(umi_workbench_designer_rect_intersects(&outer, &inner));
    TEST_REQUIRE(!umi_workbench_designer_rect_intersects(&outer, &disjoint));
    intersection = umi_workbench_designer_rect_intersection(&outer, &inner);
    TEST_REQUIRE(intersection.x == 20.0);
    TEST_REQUIRE(intersection.y == 10.0);
    TEST_REQUIRE(intersection.width == 30.0);
    TEST_REQUIRE(intersection.height == 40.0);
    TEST_REQUIRE(umi_workbench_designer_clamp(12.0, 0.0, 10.0) == 10.0);
    TEST_REQUIRE(umi_workbench_designer_clamp(-2.0, 0.0, 10.0) == 0.0);
    return 0;
}
