/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_workbench_bridge.c
 *
 * PURPOSE:
 *   Verify application presets map into concrete project generation requests.
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

#include "umicom/developer_project/workbench_bridge.h"

int main(void)
{
    UmiDeveloperWorkbenchProjectWizard wizard;
    UmiDeveloperProjectGenerationRequest request;

    umi_developer_workbench_project_wizard_init(&wizard);
    assert(umi_developer_workbench_project_wizard_select_preset(
        &wizard,
        "umicom.preset.developer-workbench") == UMI_STATUS_OK);
    assert(umi_developer_workbench_project_wizard_set_identity(
        &wizard,
        "Example Studio",
        "org.umicom.example-studio",
        "example-studio",
        "C:/work/example-studio") == UMI_STATUS_OK);
    assert(umi_developer_workbench_project_wizard_validate(&wizard) ==
           UMI_STATUS_OK);

    assert(umi_developer_project_request_from_wizard(
        &wizard, &request) == UMI_STATUS_OK);
    assert(strcmp(
        request.template_id,
        "developer.template.thin-desktop-application") == 0);
    assert(strcmp(request.target_name, "example_studio") == 0);
    return 0;
}
