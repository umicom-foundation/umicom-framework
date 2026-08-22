/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_route_preview.c
 *
 * PURPOSE:
 *   Verify route preview identifies compatible recipients before publication.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/route_preview.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostRoutePreview preview;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_route_preview_build(
        fixture.host,
        "test.source.endpoint",
        "blue",
        UMI_CONTEXT_KIND_PROJECT,
        &preview) == UMI_STATUS_OK);
    assert(preview.recipient_count == 1U);
    assert(preview.count >= 1U);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
