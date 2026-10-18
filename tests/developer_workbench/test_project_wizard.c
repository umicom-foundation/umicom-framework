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
#include "../value_archive/revision_cases.h"
#include <assert.h>

#include "umicom/developer_workbench/project_wizard.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperWorkbenchProjectWizard wizard;

    umi_developer_workbench_project_wizard_init(&wizard);


    /* Each counter must reject exhaustion before any part of this value changes. */
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiDeveloperWorkbenchProjectWizard, &wizard, revision,
        umi_developer_workbench_project_wizard_select_preset(
        &wizard,
        "umicom.preset.developer-workbench"));
    assert(umi_developer_workbench_project_wizard_select_preset(
        &wizard,
        "umicom.preset.developer-workbench") == UMI_STATUS_OK);


    /* Each counter must reject exhaustion before any part of this value changes. */
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiDeveloperWorkbenchProjectWizard, &wizard, revision,
        umi_developer_workbench_project_wizard_set_identity(
        &wizard,
        "Example IDE",
        "org.umicom.example-ide",
        "umicom-example-ide",
        "C:/work"));
    assert(umi_developer_workbench_project_wizard_set_identity(
        &wizard,
        "Example IDE",
        "org.umicom.example-ide",
        "umicom-example-ide",
        "C:/work") == UMI_STATUS_OK);


    /* Each counter must reject exhaustion before any part of this value changes. */
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiDeveloperWorkbenchProjectWizard, &wizard, revision,
        umi_developer_workbench_project_wizard_validate(&wizard));
    assert(umi_developer_workbench_project_wizard_validate(&wizard) ==
           UMI_STATUS_OK);
    assert(wizard.ready == 1);
    assert(umi_developer_workbench_project_wizard_preset(&wizard) != NULL);

    return 0;
}
