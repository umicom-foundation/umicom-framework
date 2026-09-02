/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_workbench.c
 *
 * PURPOSE:
 *   Verify the reusable workbench composes commands, search and perspectives
 *   while leaving unbound product actions disabled.
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

#include "umicom/developer_workbench/workbench.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiToolchainProfile profile;
    UmiDeveloperWorkbenchBindings bindings;
    UmiDeveloperWorkbench *workbench = NULL;
    UmiDeveloperWorkbenchSnapshot snapshot;
    UmiDeveloperWorkbenchSearchSession search;

    umi_toolchain_profile_init(&profile);
    (void)memset(&bindings, 0, sizeof(bindings));
    bindings.toolchain = &profile;

    assert(umi_developer_workbench_create(
        &bindings, &workbench) == UMI_STATUS_OK);

    assert(!umi_developer_workbench_command_enabled(
        workbench, "file.save", NULL));

    umi_developer_workbench_search_session_init(&search);
    assert(umi_developer_workbench_search(
        workbench, "Build", &search) == UMI_STATUS_OK);
    assert(search.result_count > 0U);

    assert(umi_developer_workbench_activate_perspective(
        workbench,
        "developer.perspective.debugging") == UMI_STATUS_OK);

    assert(umi_developer_workbench_snapshot(
        workbench, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.command_count >= 100U);
    assert(snapshot.perspective_count == 7U);
    assert(strcmp(snapshot.active_perspective_id,
                  "developer.perspective.debugging") == 0);

    umi_developer_workbench_destroy(workbench);
    return 0;
}
