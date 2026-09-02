/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_source_location_publisher.c
 *
 * PURPOSE:
 *   Verify canonical source-location publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "test_support.h"
#include "umicom/workbench_context_host/source_location_publisher.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    const UmiWorkbenchContextLinkActiveSlot *active;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish_source_location(
        fixture.host, "blue", "test.source", "source-1",
        "workspace", "C:/dev/main.c", "main", 10U, 2U, 4U, 100U) == UMI_STATUS_OK);
    active = umi_workbench_context_link_service_current(
        fixture.host->link_service, "blue");
    assert(active != NULL);
    assert(active->payload.kind == UMI_CONTEXT_KIND_SOURCE_LOCATION);
    assert(active->payload.identity.context_id[0] != '\0');
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
