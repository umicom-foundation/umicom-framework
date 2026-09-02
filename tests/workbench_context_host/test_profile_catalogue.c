/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_profile_catalogue.c
 *
 * PURPOSE:
 *   Verify reusable host profile catalogue replacement and removal.
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
#include "umicom/workbench_context_host/profile_catalogue.h"

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostProfileCatalogue catalogue;
    UmiWorkbenchContextHostTestFixture fixture;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    umi_workbench_context_host_profile_catalogue_init(&catalogue);
    assert(umi_workbench_context_host_profile_catalogue_upsert(
        &catalogue, &fixture.profile) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_profile_catalogue_find(
        &catalogue, "test.profile") != NULL);
    assert(umi_workbench_context_host_profile_catalogue_remove(
        &catalogue, "test.profile") == UMI_STATUS_OK);
    umi_workbench_context_host_profile_catalogue_destroy(&catalogue);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
