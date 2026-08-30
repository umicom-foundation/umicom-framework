/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_service.c
 *
 * PURPOSE:
 *   Verify the complete service owns built-in languages/templates/providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/service.h"

int main(void)
{
    UmiDeveloperProjectService *service = NULL;
    UmiDeveloperProjectServiceSnapshot snapshot;

    assert(umi_developer_project_service_create(&service) == UMI_STATUS_OK);
    assert(umi_developer_project_service_snapshot(
        service, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.language_count == 18U);
    assert(snapshot.template_count >= 15U);
    assert(umi_developer_project_provider_registry_count(
        umi_developer_project_service_providers(service)) == 5U);

    umi_developer_project_service_destroy(service);
    return 0;
}
