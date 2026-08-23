/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_recent_projects.c
 *
 * PURPOSE:
 *   Verify project MRU state reuses the cross-product recent-item registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_workbench/recent_projects.h"

int main(void)
{
    UmiRecentItemRegistry *registry = NULL;
    UmiRecentItemSnapshot item;

    assert(umi_platform_recent_items_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_recent_projects_add(
        registry,
        "project.alpha",
        "C:/work/alpha",
        "Alpha",
        100U) == UMI_STATUS_OK);
    assert(umi_platform_recent_items_registry_find(
        registry, "project.alpha", &item) == UMI_STATUS_OK);
    assert(strcmp(item.kind, "project") == 0);

    assert(umi_developer_workbench_recent_projects_add(
        registry,
        "project.alpha",
        "C:/work/alpha",
        "Alpha",
        200U) == UMI_STATUS_OK);
    assert(umi_platform_recent_items_registry_find(
        registry, "project.alpha", &item) == UMI_STATUS_OK);
    assert(item.open_count >= 2U);

    umi_platform_recent_items_registry_destroy(registry);
    return 0;
}
