/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_search.c
 *
 * PURPOSE:
 *   Verify endpoint search across labels, panels, applications and groups.
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
#include "umicom/workbench_context_host/search.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostSearchResults results;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_search(
        &fixture.host->endpoints, "target", &results) == UMI_STATUS_OK);
    assert(results.count == 1U);
    assert(strcmp(results.items[0].endpoint_id, "test.target.endpoint") == 0);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
