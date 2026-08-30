/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_headless.c
 *
 * PURPOSE:
 *   Verify the headless adapter renders shell, pane, status and command-backed workbench state.
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
    if (status == UMI_STATUS_OK) status = umi_ui_workbench_activate_perspective(*out_workbench, "studio.develop");
    return status;
}

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiUiHeadlessAdapter *adapter = NULL;
    UmiUiHeadlessSnapshot snapshot;
    UmiUiStatusSnapshot status = {0};
    int count = 0;
    assert(create_workbench(&commands, &workbench, &count) == UMI_STATUS_OK);
    (void)snprintf(status.item_id, sizeof(status.item_id), "%s", "status.ready");
    (void)snprintf(status.text, sizeof(status.text), "%s", "Ready");
    status.visible = 1;
    assert(umi_ui_status_model_upsert(umi_ui_workbench_status(workbench), &status) == UMI_STATUS_OK);
    assert(umi_ui_application_shell_create("org.umicom.studio", "Umicom Studio IDE", workbench, &shell) == UMI_STATUS_OK);
    assert(umi_ui_headless_adapter_create(&adapter) == UMI_STATUS_OK);
    assert(umi_ui_headless_adapter_present(adapter, shell) == UMI_STATUS_OK);
    assert(umi_ui_headless_adapter_snapshot(adapter, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.pane_count == 1U);
    assert(strstr(snapshot.text, "Umicom Studio IDE") != NULL);
    assert(strstr(snapshot.text, "studio.explorer") != NULL);
    umi_ui_headless_adapter_destroy(adapter);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return EXIT_SUCCESS;
}
