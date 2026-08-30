/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_start_centre.c
 *
 * PURPOSE:
 *   Verify Start Centre quick actions and recent projects.
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

#include "umicom/developer_workbench/recent_projects.h"
#include "umicom/developer_workbench/start_centre.h"

int main(void)
{
    UmiRecentItemRegistry *registry = NULL;
    UmiDeveloperWorkbenchStartCentreSnapshot snapshot;

    assert(umi_platform_recent_items_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_recent_projects_add(
        registry,
        "project.alpha",
        "C:/work/alpha",
        "Alpha",
        100U) == UMI_STATUS_OK);

    assert(umi_developer_workbench_start_centre_snapshot(
        registry, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.recent_count == 1U);
    assert(strcmp(snapshot.new_project_command, "project.new") == 0);
    assert(strcmp(snapshot.open_folder_command, "workspace.open-folder") == 0);

    umi_platform_recent_items_registry_destroy(registry);
    return 0;
}
