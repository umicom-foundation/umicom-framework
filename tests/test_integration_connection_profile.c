/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_connection_profile.c
 *
 * PURPOSE:
 *   Implement the test integration connection profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Connection profiles | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "integration_designer_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiIntegrationConnectionCatalogue catalogue = {0};
    UmiIntegrationConnectionProfile profile = umi_test_connection();
    UmiIntegrationDesignerValidation validation = {0};
    assert(umi_integration_connection_profile_validate(&profile,&validation) == UMI_STATUS_OK);
    assert(umi_integration_connection_catalogue_add(&catalogue,&profile) == UMI_STATUS_OK);
    assert(umi_integration_connection_catalogue_add(&catalogue,&profile) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_integration_connection_catalogue_find(&catalogue,"orders-api") != NULL);
    assert(umi_integration_connection_catalogue_remove(&catalogue,"orders-api") == UMI_STATUS_OK && catalogue.count == 0U);
    return 0;
}
