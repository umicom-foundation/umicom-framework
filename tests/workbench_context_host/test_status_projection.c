/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_status_projection.c
 *
 * PURPOSE:
 *   Verify active group, linked count and current context status.
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
#include "umicom/workbench_context_host/status_projection.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostStatusProjection projection;
    UmiContextPayload payload;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(&payload, "selection", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_status_projection_build(
        fixture.host, &projection) == UMI_STATUS_OK);
    assert(projection.has_context);
    assert(projection.linked_endpoint_count == 2U);
    assert(strcmp(projection.active_context_id, "selection") == 0);
    assert(strstr(projection.summary, "Blue") != NULL);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
