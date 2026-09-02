/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_builtin_providers.c
 *
 * PURPOSE:
 *   Verify all built-in project providers register together.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/builtin_providers.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperProjectProviderRegistry *registry = NULL;

    assert(umi_developer_project_builtin_provider_count() ==
           5U);
    assert(umi_developer_project_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_project_builtin_providers_register(registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_project_provider_registry_count(registry) ==
           5U);

    umi_developer_project_provider_registry_destroy(registry);
    return 0;
}
