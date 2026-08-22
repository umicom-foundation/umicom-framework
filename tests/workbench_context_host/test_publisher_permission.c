/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_publisher_permission.c
 *
 * PURPOSE:
 *   Verify a follow-only endpoint cannot publish context into its group.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/host.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiContextPayload payload;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(
        &payload, "selection", "test.target") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host,
        "blue",
        "test.target",
        &payload,
        100U) == UMI_STATUS_PERMISSION_DENIED);
    assert(fixture.host->metrics.error_count == 1U);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
