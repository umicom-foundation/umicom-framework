/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_host.c
 *
 * PURPOSE:
 *   Verify publication, active context, target inbox and metrics.
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
#include "test_support.h"

int main(void)
{

    UmiWorkbenchContextHostTestFixture fixture;
    UmiContextPayload payload;
    const UmiWorkbenchContextLinkActiveSlot *active;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_project_payload(
        &payload, "project-context", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 100U) == UMI_STATUS_OK);
    active = umi_workbench_context_link_service_current(
        fixture.host->link_service, "blue");
    assert(active != NULL);
    assert(active->payload.kind == UMI_CONTEXT_KIND_PROJECT);
    assert(fixture.host->metrics.publication_count == 1U);
    assert(fixture.host->metrics.delivery_queued_count == 1U);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
