/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_toolbar_projection.c
 *
 * PURPOSE:
 *   Verify context navigation and pin actions reflect host state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/toolbar_projection.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostToolbarProjection projection;
    UmiContextPayload payload;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(&payload, "selection", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_toolbar_projection_build(
        fixture.host, &projection) == UMI_STATUS_OK);
    assert(projection.count == 4U);
    assert(projection.actions[0].enabled);
    assert(projection.actions[2].enabled);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
