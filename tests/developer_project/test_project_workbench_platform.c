/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_project_workbench_platform.c
 *
 * PURPOSE:
 *   Verify New Project actions become real Framework command bindings.
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

#include "umicom/developer_project/project_workbench_platform.h"

int main(void)
{
    UmiToolchainProfile profile;
    UmiDeveloperWorkbenchBindings bindings;
    UmiDeveloperProjectWorkbenchPlatform *platform = NULL;
    UmiDeveloperWorkbench *workbench;
    char message[128];

    umi_toolchain_profile_init(&profile);
    (void)memset(&bindings, 0, sizeof(bindings));
    bindings.toolchain = &profile;

    assert(umi_developer_project_workbench_platform_create(
        &bindings, &platform) == UMI_STATUS_OK);

    workbench = umi_developer_project_workbench_platform_workbench(platform);
    assert(workbench != NULL);
    assert(umi_developer_workbench_command_enabled(
        workbench, "project.new", NULL));

    assert(umi_developer_workbench_execute_command(
        workbench,
        "project.new",
        NULL,
        message,
        sizeof(message)) == UMI_STATUS_OK);
    assert(strstr(message, "initialised") != NULL);

    umi_developer_project_workbench_platform_destroy(platform);
    return 0;
}
