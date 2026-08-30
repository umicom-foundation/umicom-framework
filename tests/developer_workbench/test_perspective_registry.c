/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_perspective_registry.c
 *
 * PURPOSE:
 *   Verify perspective registration and task-mode switching.
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

#include "umicom/developer_workbench/builtin_perspectives.h"

int main(void)
{
    UmiDeveloperWorkbenchPerspectiveRegistry *registry = NULL;
    const UmiDeveloperWorkbenchPerspectiveDefinition *active;

    assert(umi_developer_workbench_perspective_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_builtin_perspectives_register(registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_perspective_registry_activate(
        registry,
        "developer.perspective.testing") == UMI_STATUS_OK);

    active = umi_developer_workbench_perspective_registry_active(registry);
    assert(active != NULL);
    assert(strcmp(active->perspective_id,
                  "developer.perspective.testing") == 0);

    umi_developer_workbench_perspective_registry_destroy(registry);
    return 0;
}
