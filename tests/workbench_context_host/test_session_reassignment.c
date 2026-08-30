/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_session_reassignment.c
 *
 * PURPOSE:
 *   Verify session capture/restore preserves reassigned panel groups.
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
#include "umicom/workbench_context_host/session.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostSession session;
    const UmiWorkbenchContextHostEndpoint *endpoint;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_assign_endpoint_group(
        fixture.host,
        "test.target.endpoint",
        "red",
        UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_session_capture(
        fixture.host, &session) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_assign_endpoint_group(
        fixture.host,
        "test.target.endpoint",
        "blue",
        UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_session_apply(
        fixture.host, &session) == UMI_STATUS_OK);
    endpoint = umi_workbench_context_host_endpoint_registry_find_const(
        &fixture.host->endpoints, "test.target.endpoint");
    assert(endpoint != NULL);
    assert(strcmp(endpoint->group_id, "red") == 0);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
