/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ui_workspace_profile_designer.c
 *
 * PURPOSE:
 *   Verify the toolkit-neutral custom-layout lifecycle used by GTK4 and future
 *   frontends: drag, save, update, protect, restore and delete.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiUiPaneSnapshot pane(const char *id,
                              const char *title,
                              UmiUiPlacement placement,
                              int32_t order)
{
    UmiUiPaneSnapshot result = {0};
    (void)snprintf(result.pane_id, sizeof(result.pane_id), "%s", id);
    (void)snprintf(result.title, sizeof(result.title), "%s", title);
    (void)snprintf(result.view_type, sizeof(result.view_type), "%s", id);
    result.placement = placement;
    result.order = order;
    result.visible = 1;
    result.closable = 1;
    result.movable = 1;
    return result;
}

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiWorkspaceProfileSnapshot built_in = {0};
    UmiUiWorkspaceProfileSnapshot custom;
    UmiUiPaneSnapshot explorer = pane(
        "studio.explorer", "Explorer", UMI_UI_PLACEMENT_LEFT, 10);
    UmiUiPaneSnapshot output = pane(
        "studio.output", "Output", UMI_UI_PLACEMENT_BOTTOM, 10);
    char custom_id[UMI_UI_ID_CAPACITY];
    int modified = 0;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("studio.layout-test", commands,
                                   &workbench) == UMI_STATUS_OK);
    assert(umi_ui_pane_model_upsert(umi_ui_workbench_panes(workbench),
                                    &explorer) == UMI_STATUS_OK);
    assert(umi_ui_pane_model_upsert(umi_ui_workbench_panes(workbench),
                                    &output) == UMI_STATUS_OK);

    (void)snprintf(built_in.profile_id, sizeof(built_in.profile_id), "%s",
                   "develop");
    (void)snprintf(built_in.label, sizeof(built_in.label), "%s", "Develop");
    built_in.sidebar_visible = 1;
    built_in.bottom_panel_visible = 1;
    built_in.sidebar_size = 288;
    built_in.auxiliary_sidebar_size = 360;
    built_in.bottom_panel_size = 240;
    built_in.built_in = 1;
    built_in.locked = 1;
    built_in.pane_count = 2U;
    (void)snprintf(built_in.panes[0].pane_id,
                   sizeof(built_in.panes[0].pane_id), "%s", explorer.pane_id);
    built_in.panes[0].placement = explorer.placement;
    built_in.panes[0].order = explorer.order;
    built_in.panes[0].visible = 1;
    (void)snprintf(built_in.panes[1].pane_id,
                   sizeof(built_in.panes[1].pane_id), "%s", output.pane_id);
    built_in.panes[1].placement = output.placement;
    built_in.panes[1].order = output.order;
    built_in.panes[1].visible = 1;
    assert(umi_ui_workspace_profile_model_upsert(
               umi_ui_workbench_workspace_profiles(workbench), &built_in) ==
           UMI_STATUS_OK);
    assert(umi_ui_workbench_activate_workspace_profile(workbench, "develop") ==
           UMI_STATUS_OK);

    assert(umi_ui_workbench_dock_pane(workbench, "studio.output",
                                      UMI_UI_PLACEMENT_RIGHT, 30) ==
           UMI_STATUS_OK);
    assert(umi_ui_workbench_workspace_profile_modified(
               workbench, "develop", &modified) == UMI_STATUS_OK);
    assert(modified);
    assert(umi_ui_workbench_save_workspace_profile(
               workbench, "My Debug Layout", "Saved layout", custom_id,
               sizeof(custom_id)) == UMI_STATUS_OK);
    assert(strcmp(custom_id, "custom-my-debug-layout") == 0);
    assert(umi_ui_workspace_profile_model_find(
               umi_ui_workbench_workspace_profiles(workbench), custom_id,
               &custom) == UMI_STATUS_OK);
    assert(custom.active && !custom.built_in && custom.pane_count == 2U);
    assert(umi_ui_workbench_update_workspace_profile(
               workbench, custom_id, "My Debug Review", "Updated layout") ==
           UMI_STATUS_OK);
    assert(umi_ui_workbench_set_workspace_profile_locked(
               workbench, custom_id, 1) == UMI_STATUS_OK);
    assert(umi_ui_workbench_update_workspace_profile(
               workbench, custom_id, "Blocked", "Locked layout") ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ui_workbench_set_workspace_profile_locked(
               workbench, custom_id, 0) == UMI_STATUS_OK);
    assert(umi_ui_workbench_activate_workspace_profile(workbench, "develop") ==
           UMI_STATUS_OK);
    assert(umi_ui_pane_model_find(umi_ui_workbench_panes(workbench),
                                  "studio.output", &output) == UMI_STATUS_OK);
    assert(output.placement == UMI_UI_PLACEMENT_BOTTOM);
    assert(umi_ui_workbench_remove_workspace_profile(workbench, custom_id) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_count(
               umi_ui_workbench_workspace_profiles(workbench)) == 1U);

    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return EXIT_SUCCESS;
}
