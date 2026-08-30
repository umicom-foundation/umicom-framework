/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_workbench_binding.c
 *
 * PURPOSE:
 *   Verify application experience metadata is projected into the existing UI workbench models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/runtime/workspace_runtime.h"
#include "umicom/runtime/command_registry.h"
#include "umicom/ui/workbench.h"

int test_application_runtime_workbench_binding(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiApplicationWorkspaceRuntime runtime;
    UmiUiWorkbenchSnapshot snapshot;
    UmiUiContextSnapshot context;
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.trader");

    assert(experience != NULL);
    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("test.workbench", commands, &workbench) == UMI_STATUS_OK);
    assert(umi_application_workspace_runtime_init(experience, &runtime) == UMI_STATUS_OK);
    assert(umi_application_workspace_runtime_bind_workbench(&runtime, workbench) == UMI_STATUS_OK);
    assert(umi_ui_workbench_snapshot(workbench, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.pane_count == experience->panel_count);
    assert(strcmp(snapshot.active_workspace_profile, experience->default_layout_id) == 0);

    assert(umi_application_workspace_runtime_set_context(&runtime, "trading.red", "EUR.USD") == UMI_STATUS_OK);
    assert(umi_ui_context_get(umi_ui_workbench_context(workbench), "trading.red", &context) == UMI_STATUS_OK);
    assert(strcmp(context.string_value, "EUR.USD") == 0);

    umi_application_workspace_runtime_unbind_workbench(&runtime);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return 0;
}
