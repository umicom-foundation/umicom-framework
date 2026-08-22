/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_inspector_projection.c
 *
 * PURPOSE:
 *   Verify identity and generic values appear as inspector rows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/inspector_projection.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostInspectorProjection projection;
    UmiContextPayload payload;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(&payload, "selection", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_payload_add_text(
        &payload, "key", "value") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_inspector_projection_build(
        fixture.host, "blue", &projection) == UMI_STATUS_OK);
    assert(projection.available);
    assert(projection.row_count >= 6U);
    assert(strcmp(projection.context_id, "selection") == 0);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
