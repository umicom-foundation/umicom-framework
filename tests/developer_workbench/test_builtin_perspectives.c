/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_builtin_perspectives.c
 *
 * PURPOSE:
 *   Verify all built-in developer perspectives can be installed together.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/builtin_perspectives.h"

int main(void)
{
    UmiDeveloperWorkbenchPerspectiveRegistry *registry = NULL;

    assert(umi_developer_workbench_builtin_perspective_count() == 7U);
    assert(umi_developer_workbench_perspective_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_builtin_perspectives_register(registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_perspective_registry_count(registry) == 7U);
    assert(umi_developer_workbench_perspective_registry_active(registry) !=
           NULL);

    umi_developer_workbench_perspective_registry_destroy(registry);
    return 0;
}
