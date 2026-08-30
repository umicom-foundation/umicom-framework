/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_workbench.c
 *
 * PURPOSE:
 *   Verify workbench composition, perspective activation and canonical command execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static UmiStatus test_command(void *user_data, const char *argument, char *out_message, size_t capacity)
{
    int *count = (int *)user_data;
    (void)argument;
    *count += 1;
    (void)snprintf(out_message, capacity, "%s", "executed");
    return UMI_STATUS_OK;
}
static UmiStatus create_workbench(UmiCommandRegistry **out_commands, UmiUiWorkbench **out_workbench, int *count)
{
    UmiCommandDescriptor command = {0};
    UmiUiActionSnapshot action = {0};
    UmiUiPerspectiveSnapshot perspective = {0};
    UmiUiWorkspaceProfileSnapshot profile = {0};
    UmiUiPaneSnapshot pane = {0};
    UmiStatus status = umi_command_registry_create(out_commands);
    if (status != UMI_STATUS_OK) return status;
    command.structure_size = (uint32_t)sizeof(command);
    command.command_id = "studio.test";
    command.title = "Test";
    command.category = "Studio";
    command.description = "Test command";
    command.required_permission = "";
    command.handler = test_command;
    command.user_data = count;
    status = umi_command_registry_register(*out_commands, &command);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_create("studio.workbench", *out_commands, out_workbench);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", "studio.test.action");
    (void)snprintf(action.command_id, sizeof(action.command_id), "%s", "studio.test");
    (void)snprintf(action.label, sizeof(action.label), "%s", "Test");
    action.enabled = 1; action.visible = 1;
    status = umi_ui_action_model_upsert(umi_ui_workbench_actions(*out_workbench), &action);
    (void)snprintf(perspective.perspective_id, sizeof(perspective.perspective_id), "%s", "studio.develop");
    (void)snprintf(perspective.title, sizeof(perspective.title), "%s", "Develop");
    perspective.active = 1;
    if (status == UMI_STATUS_OK) status = umi_ui_perspective_model_upsert(umi_ui_workbench_perspectives(*out_workbench), &perspective);
    (void)snprintf(pane.pane_id, sizeof(pane.pane_id), "%s", "studio.explorer");
    (void)snprintf(pane.title, sizeof(pane.title), "%s", "Explorer");
    (void)snprintf(pane.view_type, sizeof(pane.view_type), "%s", "studio.project-tree");
    pane.placement = UMI_UI_PLACEMENT_LEFT; pane.visible = 1;
    if (status == UMI_STATUS_OK) status = umi_ui_pane_model_upsert(umi_ui_workbench_panes(*out_workbench), &pane);
    (void)snprintf(profile.profile_id, sizeof(profile.profile_id), "%s", "focus");
    (void)snprintf(profile.label, sizeof(profile.label), "%s", "Focus");
    profile.sidebar_visible = 0;
    profile.auxiliary_sidebar_visible = 0;
    profile.bottom_panel_visible = 0;
    profile.sidebar_size = 280;
    profile.auxiliary_sidebar_size = 360;
    profile.bottom_panel_size = 240;
    profile.editor_split_mode = UMI_UI_EDITOR_SPLIT_ROWS;
    profile.editor_split_ratio = 6100;
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_profile_model_upsert(
            umi_ui_workbench_workspace_profiles(*out_workbench), &profile);
    }
    if (status == UMI_STATUS_OK) status = umi_ui_workbench_activate_perspective(*out_workbench, "studio.develop");
    return status;
}

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiWorkbenchSnapshot snapshot;
    char message[64];
    int count = 0;
    assert(create_workbench(&commands, &workbench, &count) == UMI_STATUS_OK);
    assert(umi_ui_workbench_execute_action(workbench, "studio.test.action", "", message, sizeof(message)) == UMI_STATUS_OK);
    assert(count == 1 && strcmp(message, "executed") == 0);
    assert(umi_ui_workbench_activate_workspace_profile(workbench, "focus") ==
           UMI_STATUS_OK);
    assert(umi_ui_workbench_snapshot(workbench, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.pane_count == 1U && snapshot.action_count == 1U);
    assert(snapshot.workspace_profile_count == 1U);
    assert(strcmp(snapshot.active_workspace_profile, "focus") == 0);
    {
        UmiUiWorkbenchState state;
        UmiUiDocumentViewSnapshot primary = {0};
        UmiUiDocumentViewSnapshot secondary = {0};
        (void)snprintf(primary.view_id, sizeof(primary.view_id), "%s",
                       "document.primary");
        (void)snprintf(primary.title, sizeof(primary.title), "%s", "Primary");
        (void)snprintf(primary.group_id, sizeof(primary.group_id), "%s",
                       UMI_UI_PRIMARY_EDITOR_GROUP_ID);
        primary.active = 1;
        (void)snprintf(secondary.view_id, sizeof(secondary.view_id), "%s",
                       "document.secondary");
        (void)snprintf(secondary.title, sizeof(secondary.title), "%s",
                       "Secondary");
        (void)snprintf(secondary.group_id, sizeof(secondary.group_id), "%s",
                       UMI_UI_SECONDARY_EDITOR_GROUP_ID);
        secondary.active = 1;
        assert(umi_ui_document_view_model_upsert(
                   umi_ui_workbench_documents(workbench), &primary) ==
               UMI_STATUS_OK);
        assert(umi_ui_document_view_model_upsert(
                   umi_ui_workbench_documents(workbench), &secondary) ==
               UMI_STATUS_OK);
        assert(umi_ui_workbench_activate_document(
                   workbench, secondary.view_id) == UMI_STATUS_OK);
        assert(umi_ui_document_view_model_find(
                   umi_ui_workbench_documents(workbench), primary.view_id,
                   &primary) == UMI_STATUS_OK);
        assert(primary.active);
        assert(umi_ui_workbench_state_snapshot(workbench, &state) ==
               UMI_STATUS_OK);
        assert(strcmp(state.active_editor_group,
                      UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0);
        assert(state.editor_split_mode == UMI_UI_EDITOR_SPLIT_ROWS);
        assert(state.editor_split_ratio == 6100);
    }
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return EXIT_SUCCESS;
}
