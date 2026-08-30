/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_project_wizard.c
 *
 * PURPOSE:
 *   Verify project wizard planning reuses Framework application presets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/project_wizard.h"

int main(void)
{
    UmiDeveloperWorkbenchProjectWizard wizard;

    umi_developer_workbench_project_wizard_init(&wizard);

    assert(umi_developer_workbench_project_wizard_select_preset(
        &wizard,
        "umicom.preset.developer-workbench") == UMI_STATUS_OK);

    assert(umi_developer_workbench_project_wizard_set_identity(
        &wizard,
        "Example IDE",
        "org.umicom.example-ide",
        "umicom-example-ide",
        "C:/work") == UMI_STATUS_OK);

    assert(umi_developer_workbench_project_wizard_validate(&wizard) ==
           UMI_STATUS_OK);
    assert(wizard.ready == 1);
    assert(umi_developer_workbench_project_wizard_preset(&wizard) != NULL);

    return 0;
}
