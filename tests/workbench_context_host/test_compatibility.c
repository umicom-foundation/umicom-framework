/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_compatibility.c
 *
 * PURPOSE:
 *   Verify source, group and follower compatibility reaches a full score.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/compatibility.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostCompatibility compatibility;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_compatibility_evaluate(
        fixture.host,
        "test.source.endpoint",
        "test.target.endpoint",
        "blue",
        UMI_CONTEXT_KIND_PROJECT,
        &compatibility) == UMI_STATUS_OK);
    assert(compatibility.compatible);
    assert(compatibility.score == 100);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
