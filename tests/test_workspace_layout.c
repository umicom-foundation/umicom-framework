/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_layout.c
 *
 * PURPOSE:
 *   Implement the test workspace layout behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Professional layout tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/ui/workspace_geometry.h"
#include "umicom/ui/workspace_layout.h"
/* Compare normalised geometry without requiring a floating-point library. */
static int geometry_equal(double left, double right)
{
    double difference = left >= right ? left - right : right - left;

    return difference < 0.000001;
}

/* Prove every application receives one centre-dominant default geometry map. */
static void assert_default_workspace_geometry(void)
{
    UmiUiWorkspaceRect left =
        umi_ui_workspace_region_rect(UMI_UI_PLACEMENT_LEFT);
    UmiUiWorkspaceRect centre =
        umi_ui_workspace_region_rect(UMI_UI_PLACEMENT_CENTRE);
    UmiUiWorkspaceRect right =
        umi_ui_workspace_region_rect(UMI_UI_PLACEMENT_RIGHT);
    UmiUiWorkspaceRect bottom =
        umi_ui_workspace_region_rect(UMI_UI_PLACEMENT_BOTTOM);
    UmiUiWorkspaceRect floating =
        umi_ui_workspace_region_rect(UMI_UI_PLACEMENT_FLOATING);

    assert(geometry_equal(left.x, 0.0));
    assert(geometry_equal(left.width, 0.18));
    assert(geometry_equal(centre.x, 0.18));
    assert(geometry_equal(centre.width, 0.64));
    assert(geometry_equal(right.x, 0.82));
    assert(geometry_equal(right.width, 0.18));
    assert(centre.width > left.width);
    assert(centre.width > right.width);
    assert(geometry_equal(
        left.width + centre.width + right.width, 1.0));
    assert(geometry_equal(bottom.y, 0.76));
    assert(geometry_equal(bottom.height, 0.24));
    assert(floating.x >= 0.0 && floating.y >= 0.0);
    assert(floating.x + floating.width <= 1.0);
    assert(floating.y + floating.height <= 1.0);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiWorkspaceLayout layout;

    assert_default_workspace_geometry();
    /* Named fields keep this contract test readable and prevent a newly added
     * workspace field from shifting older values into the wrong member. */
    UmiUiWorkspaceWindow editor = {
        .window_id = "editor-1",
        .title = "Editor",
        .tool_id = "editor",
        .group_id = "",
        .x = 0.2,
        .y = 0.0,
        .width = 0.8,
        .height = 0.75,
        .visible = true,
        .floating = false,
        .maximised = false,
        .closable = true,
        .z_order = 1,
        .placement_id = "",
        .stack_id = "",
        .context_group_id = "",
        .pinned = false,
        .resizable = true
    };
    char reason[192U];
    assert(umi_ui_workspace_layout_init(&layout,"develop","Develop") == UMI_STATUS_OK);
    assert(layout.locked);
    assert(umi_ui_workspace_layout_add_window(&layout,&editor) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ui_workspace_layout_set_locked(&layout,false) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_add_window(&layout,&editor) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_place_window(&layout,"editor-1",0.0,0.0,1.0,1.0) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_placement(
        &layout, "editor-1", "centre") == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_stack(
        &layout, "editor-1", "documents") == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_context_group(
        &layout, "editor-1", "project-blue") == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_pinned(
        &layout, "editor-1", true) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_maximised(
        &layout,"editor-1",true) == UMI_STATUS_OK);
    assert(strcmp(layout.windows[0].placement_id, "centre") == 0);
    assert(strcmp(layout.windows[0].stack_id, "documents") == 0);
    assert(strcmp(layout.windows[0].context_group_id, "project-blue") == 0);
    assert(layout.windows[0].pinned);
    assert(umi_ui_workspace_layout_set_locked(&layout,true) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_maximised(
        &layout, "editor-1", false) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ui_workspace_layout_validate(&layout,reason,sizeof(reason)) == UMI_STATUS_OK);
    assert(strcmp(reason,"Layout is valid") == 0);
    return 0;
}
